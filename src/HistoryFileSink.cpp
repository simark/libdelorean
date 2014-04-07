/**
 * Copyright (c) 2014 Philippe Proulx <eepp.ca>
 * Copyright (c) 2012 François Rajotte <francois.rajotte@polymtl.ca>
 *
 * This file is part of libdelorean.
 *
 * libdelorean is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libdelorean is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libdelorean.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <cstdint>
#include <memory>
#include <vector>
#include <iostream>
#include <fstream>
#include <boost/filesystem/fstream.hpp>

#include <delorean/HistoryFileSink.hpp>
#include <delorean/AbstractHistoryFile.hpp>
#include <delorean/interval/AbstractInterval.hpp>
#include <delorean/node/LeafNode.hpp>
#include <delorean/node/BranchNode.hpp>
#include <delorean/ex/IO.hpp>
#include <delorean/ex/IntervalOutOfRange.hpp>
#include <delorean/BasicTypes.hpp>
#include "node/NodeStructs.hpp"

namespace fs = boost::filesystem;

HistoryFileSink::HistoryFileSink()
{
    // build node serializer
    std::unique_ptr<IntervalSerializer> intervalSer {new IntervalSerializer()};
    std::unique_ptr<NodeSerializer> nodeSer {
        new NodeSerializer {std::move(intervalSer)}
    };
    _nodeSer = std::move(nodeSer);
}

void HistoryFileSink::open(const fs::path& path, unsigned int nodeSize,
                           unsigned int maxChildren, timestamp_t begin)
{
    if (this->isOpened()) {
        throw IO("Trying to open a file already opened");
    }

    // try opening output stream
    _outputStream.open(path, std::ios::binary);
    if (!_outputStream) {
        throw IO("Cannot open file for writing");
    }

    // allocate new buffer for node serialization
    std::unique_ptr<std::uint8_t[]> newBuf {new std::uint8_t[nodeSize]};
    _nodeBuf = std::move(newBuf);

    // clear latest branch
    this->getLatestBranch().clear();

    // set/reset attributes
    this->setPath(path);
    this->setNodeSize(nodeSize);
    this->setMaxChildren(maxChildren);
    this->setBegin(begin);
    this->setEnd(begin);
    this->setNodeCount(0);
    this->setOpened(true);

    // add a first leaf node to latest branch (current root node)
    auto leafNode =
        std::move(this->createLeafNode(NodeCommonHeader::ROOT_SEQ_NUMBER(),
                                       begin));
    this->getLatestBranch().push_back(std::move(leafNode));
}

void HistoryFileSink::writeHeader()
{
    // seek back to offset 0
    _outputStream.seekp(0);

    // prepare header
    HistoryFileHeader header;
    header.nodeSize = this->getNodeSize();
    header.maxChildren = this->getMaxChildren();
    header.nodeCount = this->getNodeCount();
    header.rootNodeSeqNumber = this->getRootNodeSeqNumber();

    // write header
    _outputStream.write(reinterpret_cast<char*>(&header), sizeof(header));
}

void HistoryFileSink::close(timestamp_t end)
{
    if (!this->isOpened()) {
        // ignore silently
        return;
    }

    // write header now
    this->writeHeader();

    // close everything now
    _outputStream.close();
    this->setOpened(false);
}

void HistoryFileSink::close()
{
    if (!this->isOpened()) {
        // ignore silently
        return;
    }

    this->close(this->getEnd());
}

void HistoryFileSink::addInterval(AbstractInterval::SP interval)
{
    // check range
    if (interval->getBegin() < this->getBegin() ||
            interval->getEnd() < this->getEnd()) {
        throw IntervalOutOfRange {*interval, this->getBegin(), this->getEnd()};
    }
}

BranchNode::UP HistoryFileSink::createBranchNode(node_seq_t parentSeqNumber,
                                                  timestamp_t begin) const
{
    auto node = new BranchNode {
        this->getNodeSize(),
        this->getNodeCount(),
        parentSeqNumber,
        begin,
        this->getMaxChildren()
    };
    BranchNode::UP nodeUp {node};

    return nodeUp;
}

LeafNode::UP HistoryFileSink::createLeafNode(node_seq_t parentSeqNumber,
                                             timestamp_t begin) const
{
    auto node = new LeafNode {
        this->getNodeSize(),
        this->getNodeCount(),
        parentSeqNumber,
        begin
    };
    LeafNode::UP nodeUp {node};

    return nodeUp;
}

void HistoryFileSink::incNodeCount(timestamp_t begin)
{
    // one more node
    this->setNodeCount(this->getNodeCount() + 1);

    // update tree's end if needed
    if (begin >= this->getEnd()) {
        this->setEnd(begin);
    }
}

void HistoryFileSink::tryAddIntervalToNode(AbstractInterval::SP intr,
                                           unsigned int index)
{
    // target node
    auto targetNode = this->getLatestBranch().at(index);

    // TODO
}

void HistoryFileSink::addSiblingNode(unsigned int index)
{
    /* We're in a situation like this (latest branch):
     *
     *     o
     *      \
     *       o
     *       |
     *       o <- index
     *       |
     *       o
     *        \
     *         o
     *
     * `index` points to a node which should have a new sibling node. We
     * need to discard this node and all its descendants.
     *
     *     o
     *      \
     *       o <- index - 1
     *
     * and draw the latest branch down to the height it had previously:
     *
     *     o
     *      \
     *       o <- index - 1
     *        \
     *         o <- index (new sibling node)
     *          \
     *           o <- new node
     *            \
     *             o <- new node
     *
     * `index` could point to a branch node of which the parent has reached
     * its maximum number of children. In this case, we need to add a sibling
     * node to its parent.
     *
     * Also, `index` could be 0, in which case we need to create a new root
     * and apply what's written above.
     */

    unsigned int newHeight = this->getLatestBranch().size();
    unsigned int parentIndex;

    // do we need a new root?
    if (index == 0) {
        // new height: one more level
        newHeight++;

        // old root
        auto oldRootNode = this->getLatestBranch().front();

        // create new root
        auto newRootNode =
            std::move(this->createBranchNode(NodeCommonHeader::ROOT_SEQ_NUMBER(),
                                             oldRootNode->getBegin()));

        // set new root as old root's parent
        oldRootNode->setParentSeqNumber(newRootNode->getSeqNumber());

        // set old root as new root's first child
        newRootNode->addChild(*oldRootNode);

        // commit old root and all its descendants
        this->commitNodesDownFromIndex(0);

        // make new root the only item in latest branch
        this->getLatestBranch().clear();
        this->getLatestBranch().push_back(std::move(newRootNode));

        // parent index is root
        parentIndex = 0;
    } else {
        // pointed node
        auto node = this->getLatestBranch().at(index);

        // parent (must be a branch node)
        auto& parent =
            dynamic_cast<BranchNode&>(*(this->getLatestBranch().at(index - 1)));

        // is parent full?
        if (parent.isFull()) {
            // retry with parent
            this->addSiblingNode(index - 1);
        }

        // commit pointed node and all its descendant
        this->commitNodesDownFromIndex(index);

        // parent index
        parentIndex = index - 1;
    }

    // draw new part of latest branch
    this->drawBranchFromIndex(parentIndex, newHeight);
}

void HistoryFileSink::commitNode(AbstractNode& node)
{
    // seek output stream to the right offset
    _outputStream.seekp(HistoryFileHeader::SIZE +
                        this->getNodeSize() * node.getSeqNumber());

    // close node with this tree's end
    node.close(this->getEnd());

    // serialize node to buffer
    _nodeSer->serializeNode(node, _nodeBuf.get());

    // write buffer
    _outputStream.write(reinterpret_cast<char*>(_nodeBuf.get()),
                        this->getNodeSize());
}

void HistoryFileSink::commitNodesDownFromIndex(unsigned int index)
{
    for (unsigned int i = index; i < this->getLatestBranch().size(); ++i) {
        auto node = this->getLatestBranch().at(i);

        // commit it
        this->commitNode(*node);
    }
}

void HistoryFileSink::drawBranchFromIndex(unsigned int parentIndex,
                                          unsigned int height)
{
    /* `parentIndex` points to some node:
     *
     *     o
     *      \
     *       o <- parentIndex
     *        \
     *         o
     *          \
     *           o
     *            \
     *             o
     *
     * From there we want to resize the latest branch to fit only the
     * parent and its ancestors,
     *
     *     o
     *      \
     *       o <- parentIndex
     *
     * and then draw a branch down to a leaf (adding as many branch nodes
     * as needed in between) so as to get the requested height, `height`:
     *
     *     o
     *      \
     *       o <- parentIndex
     *        \
     *         o <- new branch node
     *          \
     *           o <- new branch node
     *            \
     *             o <- new leaf node
     */

    // number of nodes to add
    auto totalNewNodes = height - parentIndex - 1;

    // number of branch nodes to add
    auto newBranchNodes = totalNewNodes - 1;

    // resize latest branch to remove discarded nodes (descendants of parent)
    this->getLatestBranch().resize(parentIndex + 1);
    if (totalNewNodes == 0) {
        // seems like we don't need any new child
        return;
    }

    // first parent node will be the pointed parent
    auto& absParentNode = *(this->getLatestBranch().at(parentIndex));

    /* This dynamic cast should always work because the caller should never
     * provide the index of a leaf node (makes no sense to do so anyway).
     */
    auto& parentNode = dynamic_cast<BranchNode&>(absParentNode);

    // add branch nodes
    for (unsigned int i = 0; i < newBranchNodes; ++i) {
        // create branch node
        auto branchNode =
            std::move(this->createBranchNode(parentNode.getSeqNumber(),
                                             parentNode.getBegin()));

        // set as current parent's child
        parentNode.addChild(*branchNode);

        // make it the new parent
        parentNode = *branchNode;

        // append to latest branch
        this->getLatestBranch().push_back(std::move(branchNode));
    }

    // add single leaf node
    auto leafNode =
        std::move(this->createLeafNode(parentNode.getSeqNumber(),
                                       parentNode.getBegin()));
    parentNode.addChild(*leafNode);
    this->getLatestBranch().push_back(std::move(leafNode));
}
