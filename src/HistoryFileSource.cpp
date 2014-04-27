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
#include <memory>
#include <functional>
#include <fstream>
#include <boost/filesystem/fstream.hpp>

#include <delorean/node/AbstractNodeCache.hpp>
#include <delorean/node/PassThroughNodeCache.hpp>
#include <delorean/node/AlignedNodeSerDes.hpp>
#include <delorean/ex/IO.hpp>
#include <delorean/AbstractHistory.hpp>
#include <delorean/HistoryFileSource.hpp>


namespace bfs = boost::filesystem;

HistoryFileSource::HistoryFileSource()
{
}

void HistoryFileSource::open(const boost::filesystem::path& path,
                             std::shared_ptr<AbstractNodeCache> nodeCache)
{
    if (this->isOpened()) {
        throw IO("Trying to open a history file already opened");
    }

    // try opening input stream
    _inputStream.open(path, std::ios::binary);
    if (!_inputStream) {
        throw IO("Cannot open history file for reading");
    }

    // make sure file is at least as large as its header
    _inputStream.seekg(0, std::ifstream::end);
    if (_inputStream.tellg() < HistoryFileHeader::SIZE) {
        _inputStream.close();
        throw IO("History file is too small");
    }

    // read header
    this->readHeader();

    /* Allocate new buffer for node deserialization (reallocating because
     * its size could have changed).
     */
    auto buf = new std::uint8_t[this->getNodeSize()];
    std::unique_ptr<std::uint8_t[]> newBuf {buf};
    _nodeBuf = std::move(newBuf);

    // set cache
    if (nodeCache == nullptr) {
        nodeCache = std::shared_ptr<AbstractNodeCache> {
            new PassThroughNodeCache {}
        };
    }
    /*auto getNodeFromOwnerCb = std::bind(&HistoryFileSource::getNode, this,
                                        std::placeholders::_1);
    nodeCache->setGetNodeFromOwnerCb(getNodeFromOwnerCb);*/

    // set/reset attributes
    this->setPath(path);
    this->setOpened(true);

    // get root node now
}

void HistoryFileSource::close()
{
}

void HistoryFileSource::readHeader()
{
    // seek to offset 0
    _inputStream.seekg(0);

    // read header
    HistoryFileHeader header;
    _inputStream.read(reinterpret_cast<char*>(&header), sizeof(header));

    // make sure we recognize the magic (and set node ser/des)
    if (header.magic == HistoryFileHeader::MAGIC_ALIGNED_NODE_SERDES) {
        std::unique_ptr<AlignedNodeSerDes> serdes {new AlignedNodeSerDes};
        this->setNodeSerDes(std::move(serdes));
    } else {
        throw IO("Unknown history file magic number");
    }

    // set other parameters
    this->setNodeSize(header.nodeSize);
    this->setMaxChildren(header.maxChildren);
    this->setNodeCount(header.nodeCount);
    this->setRootNodeSeqNumber(header.rootNodeSeqNumber);
}

Node::SP HistoryFileSource::getNode(node_seq_t seqNumber)
{
    // make sure the node exists
    if (seqNumber > this->getNodeCount()) {
        return nullptr;
    }

    // seek input stream to the right offset
    _inputStream.seekg(HistoryFileHeader::SIZE +
                       this->getNodeSize() * seqNumber);

    // read node bytes
    _inputStream.read(reinterpret_cast<char*>(_nodeBuf.get()),
                      this->getNodeSize());

    // deserialize node into buffer
    auto node = this->getNodeSerDes().deserializeNode(_nodeBuf.get(),
                                                      this->getNodeSize(),
                                                      this->getMaxChildren());
    Node::SP nodeSp = std::move(node);

    return nodeSp;
}

Node::SP HistoryFileSource::getRootNode()
{
    return this->getNode(this->getRootNodeSeqNumber());
}

bool HistoryFileSource::findAll(timestamp_t ts, IntervalJar& intervals) const
{
    return false;
}

AbstractInterval::SP HistoryFileSource::findOne(timestamp_t ts,
                                                interval_key_t key) const
{
    return nullptr;
}
