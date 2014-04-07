/**
 * Copyright (c) 2014 Philippe Proulx <eepp.ca>
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
#ifndef _HISTORYFILESINK_HPP
#define _HISTORYFILESINK_HPP

#include <cstdint>
#include <memory>
#include <vector>
#include <boost/filesystem/fstream.hpp>

#include <delorean/AbstractHistoryFile.hpp>
#include <delorean/AbstractHistorySink.hpp>
#include <delorean/node/NodeSerializer.hpp>
#include <delorean/node/LeafNode.hpp>
#include <delorean/node/BranchNode.hpp>
#include <delorean/interval/AbstractInterval.hpp>
#include <delorean/BasicTypes.hpp>

class HistoryFileSink :
    public AbstractHistoryFile,
    public AbstractHistorySink
{
public:
    enum {
        DEF_NODE_SIZE = (16 * 1024),
        DEF_MAX_CHILDREN = 50
    };

public:
    HistoryFileSink();
    virtual ~HistoryFileSink()
    {
        this->close();
    }

    void open(const boost::filesystem::path& path,
              unsigned int nodeSize = DEF_NODE_SIZE,
              unsigned int maxChildren = DEF_MAX_CHILDREN,
              timestamp_t begin = 0);
    void close(timestamp_t end);
    void close();

    void addInterval(AbstractInterval::SP i);

private:
    void writeHeader();
    void tryAddIntervalToNode(AbstractInterval::SP intr, unsigned int index);
    void incNodeCount(timestamp_t begin);
    void addSiblingNode(unsigned int index);
    void drawBranchFromIndex(unsigned int parentIndex,
                             unsigned int height);
    void commitNodesDownFromIndex(unsigned int index);
    void commitNode(AbstractNode& node);
    BranchNode::UP createBranchNode(node_seq_t parentSeqNumber,
                                    timestamp_t begin) const;
    LeafNode::UP createLeafNode(node_seq_t parentSeqNumber,
                                timestamp_t begin) const;

private:
    boost::filesystem::ofstream _outputStream;
    std::unique_ptr<NodeSerializer> _nodeSer;
    std::unique_ptr<std::uint8_t[]> _nodeBuf;
};

#endif // _HISTORYFILESINK_HPP
