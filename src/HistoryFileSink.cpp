/* Copyright (c) 2014 Philippe Proulx <eepp.ca>
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
#include <delorean/node/Node.hpp>
#include <delorean/node/NodeSerDesType.hpp>
#include <delorean/node/AlignedNodeSerDes.hpp>
#include <delorean/ex/IO.hpp>
#include <delorean/ex/IntervalOutOfRange.hpp>
#include <delorean/ex/TimestampOutOfRange.hpp>
#include <delorean/ex/UnknownNodeSerDesType.hpp>
#include <delorean/BasicTypes.hpp>

namespace fs = boost::filesystem;

HistoryFileSink::HistoryFileSink()
{
}

void HistoryFileSink::open(const fs::path& path, std::size_t nodeSize,
                           std::size_t maxChildren, timestamp_t begin,
                           NodeSerDesType serdesType)
{
    if (this->isOpened()) {
        throw IO("Trying to open a history file already opened");
    }

    // try opening output stream
    _outputStream.open(path, std::ios::binary);
    if (!_outputStream) {
        throw IO("Cannot open history file for writing");
    }

    // set node serializer/deserializer
    // TODO: use factory when we have too many types
    if (serdesType == NodeSerDesType::ALIGNED) {
        AbstractNodeSerDes::UP nodeSerdes {new AlignedNodeSerDes {}};
        this->setNodeSerDes(std::move(nodeSerdes));
        _magic = HistoryFileHeader::MAGIC_ALIGNED_NODE_SERDES;
    } else {
        throw UnknownNodeSerDesType(serdesType);
    }

    /* Allocate new buffer for node serialization (reallocating because
     * its size could have changed).
     */
    std::unique_ptr<std::uint8_t[]> newBuf {new std::uint8_t[nodeSize]};
    _nodeBuf = std::move(newBuf);

    // clear latest branch (this will also free contained nodes)
    _latestBranch.clear();

    // set/reset attributes
    this->setPath(path);
    this->setNodeSize(nodeSize);
    this->setMaxChildren(maxChildren);
    this->setBegin(begin);
    this->setEnd(begin);
    this->setLastIntervalEnd(begin);
    this->setNodeCount(0);
    this->setOpened(true);

    // add a first leaf node to latest branch (current root node)
    auto rootNode = this->createLeafNode(Node::ROOT_PARENT_SEQ_NUMBER(),
                                         begin);
    this->setRootNodeSeqNumber(rootNode->getSeqNumber());
    _latestBranch.push_back(std::move(rootNode));
}

void HistoryFileSink::writeHeader()
{
    // seek back to offset 0
    _outputStream.seekp(0);

    // prepare header
    HistoryFileHeader header;
    header.magic = static_cast<uint32_t>(_magic);
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

    // set final end timestamp
    if (end > this->getEnd()) {
        this->setEnd(end);
    }

    // close latest branch
    this->commitNodesDownFromIndex(0);

    // write header now
    this->writeHeader();

    // close everything now
    _outputStream.close();
    this->setOpened(false);
}

void HistoryFileSink::close()
{
    this->close(this->getEnd());
}

void HistoryFileSink::addInterval(AbstractInterval::SP interval)
{
    // check if opened
    if (!this->isOpened()) {
        throw IO("Adding an interval to a close history file sink");
    }

    // check range
    if (interval->getBegin() < this->getBegin()) {
        throw IntervalOutOfRange {
            *interval,
            this->getBegin(),
            this->getLastIntervalEnd()
        };
    }
    if (interval->getEnd() < this->getLastIntervalEnd()) {
        throw TimestampOutOfRange {
            this->getBegin(),
            this->getLastIntervalEnd(),
            interval->getEnd()
        };
    }

    // update end times
    this->setLastIntervalEnd(interval->getEnd());
    if (interval->getEnd() > this->getEnd()) {
        this->setEnd(interval->getEnd());
    }

    // try inserting it in current leaf node
    this->tryAddIntervalToNode(interval, _latestBranch.size() - 1);
}

Node::UP HistoryFileSink::createBranchNode(node_seq_t parentSeqNumber,
                                           timestamp_t begin)
{
    Node::UP nodeUp {new Node {
        this->getNodeSize(),
        this->getMaxChildren(),
        static_cast<node_seq_t>(this->getNodeCount()),
        parentSeqNumber,
        begin,
        this->getNodeSerDesPtr()
    }};
    this->incNodeCount();

    return nodeUp;
}

Node::UP HistoryFileSink::createLeafNode(node_seq_t parentSeqNumber,
                                         timestamp_t begin)
{
    // a leaf node doesn't have children
    Node::UP nodeUp {new Node {
        this->getNodeSize(),
        0,
        static_cast<node_seq_t>(this->getNodeCount()),
        parentSeqNumber,
        begin,
        this->getNodeSerDesPtr()
    }};
    this->incNodeCount();

    return nodeUp;
}

void HistoryFileSink::tryAddIntervalToNode(AbstractInterval::SP intr,
                                           std::size_t index)
{
    // target node
    auto& targetNode = _latestBranch.at(index);

    // does this interval fits the target node?
    if (!targetNode->intervalFits(*intr)) {
        // nope: add to a new leaf sibling instead
        this->addSiblingNode(index);
        this->tryAddIntervalToNode(intr, _latestBranch.size() - 1);
        return;
    }

    // make sure the interval time range fits the target node
    if (intr->getBegin() < targetNode->getBegin()) {
        // it doesn't: check if it fits its parent
        this->tryAddIntervalToNode(intr, index - 1);
        return;
    }

    // seems like we found the proper target node
    targetNode->addInterval(intr);
}

void HistoryFileSink::addSiblingNode(std::size_t index)
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
     * need to discard this node and all its descendants:
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
    std::size_t newHeight = _latestBranch.size();
    std::size_t parentIndex;

    // do we need a new root?
    if (index == 0) {
        // new height: one more level
        newHeight++;

        // old root
        auto oldRootNode = _latestBranch.front();

        // create new root
        auto newRootNode =
            this->createBranchNode(Node::ROOT_PARENT_SEQ_NUMBER(),
                                   this->getBegin());
        this->setRootNodeSeqNumber(newRootNode->getSeqNumber());

        // set new root as old root's parent
        oldRootNode->setParentSeqNumber(newRootNode->getSeqNumber());

        // set old root as new root's first child
        newRootNode->addChild(oldRootNode->getBegin(),
                              oldRootNode->getSeqNumber());

        // commit old root and all its descendants
        this->commitNodesDownFromIndex(0);

        // make new root the only item in latest branch
        _latestBranch.clear();
        _latestBranch.push_back(std::move(newRootNode));

        // parent index is root
        parentIndex = 0;
    } else {
        // parent
        const auto& parent = *(_latestBranch.at(index - 1));

        // is parent full?
        if (parent.isFull()) {
            // retry with parent
            this->addSiblingNode(index - 1);
            return;
        }

        // commit pointed node and all its descendant
        this->commitNodesDownFromIndex(index);

        // parent index
        parentIndex = index - 1;
    }

    // draw new part of latest branch
    this->drawBranchFromIndex(parentIndex, newHeight);
}

void HistoryFileSink::commitNode(Node& node)
{
    // seek output stream to the right offset
    _outputStream.seekp(HistoryFileHeader::SIZE +
                        this->getNodeSize() * node.getSeqNumber());

    // close node with this tree's end
    node.close(this->getEnd());

    // serialize node to buffer
    this->getNodeSerDes().serializeNode(node, _nodeBuf.get());

    // write buffer
    _outputStream.write(reinterpret_cast<char*>(_nodeBuf.get()),
                        this->getNodeSize());
}

void HistoryFileSink::commitNodesDownFromIndex(std::size_t index)
{
    auto& lb = _latestBranch;
    for (auto it = lb.begin() + index; it != lb.end(); ++it) {
        this->commitNode(**it);
    }
}

void HistoryFileSink::drawBranchFromIndex(std::size_t parentIndex,
                                          std::size_t height)
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
    _latestBranch.resize(parentIndex + 1);
    if (totalNewNodes == 0) {
        // seems like we don't need any new child
        return;
    }

    // update tree's end
    this->setEnd(this->getEnd() + 1);

    // first parent node will be the pointed parent
    auto parentNode = _latestBranch.at(parentIndex).get();

    // add branch nodes
    for (std::size_t i = 0; i < newBranchNodes; ++i) {
        // create branch node
        auto branchNode = this->createBranchNode(parentNode->getSeqNumber(),
                                                 this->getEnd());

        // set as current parent's child
        parentNode->addChild(branchNode->getBegin(),
                             branchNode->getSeqNumber());

        // make it the new parent
        parentNode = branchNode.get();

        // append to latest branch
        _latestBranch.push_back(std::move(branchNode));
    }

    // add single leaf node
    auto leafNode = this->createLeafNode(parentNode->getSeqNumber(),
                                         this->getEnd());
    parentNode->addChild(leafNode->getBegin(), leafNode->getSeqNumber());
    _latestBranch.push_back(std::move(leafNode));
}
