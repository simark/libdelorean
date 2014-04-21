/* Copyright (c) 2014 Philippe Proulx <eepp.ca>
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
#include <delorean/IHistorySink.hpp>
#include <delorean/node/Node.hpp>
#include <delorean/node/NodeSerDesType.hpp>
#include <delorean/interval/AbstractInterval.hpp>
#include <delorean/BasicTypes.hpp>

/**
 * History file opened for output. Use an HistoryFileSink object to build a
 * brand new history file.
 *
 * @see HistoryFileSource
 * @author Philippe Proulx
 */
class HistoryFileSink :
    public AbstractHistoryFile,
    public IHistorySink
{
public:
    /// Default values used when opening the file
    enum {
        /// Default node size in bytes
        DEF_NODE_SIZE = (16 * 1024),

        /// Default maximum number of children in a node
        DEF_MAX_CHILDREN = 50
    };

public:
    /**
     * Builds a history file sink. The file is closed and need to be opened
     * with proper parameters in order to start adding intervals.
     */
    HistoryFileSink();

    virtual ~HistoryFileSink()
    {
        this->close();
    }

    /**
     * Opens the history file for writing.
     *
     * @param path        Path to history file to be created
     * @param nodeSize    Size of each single node
     * @param maxChildren Maximum number of children in a node
     * @param begin       Begin timestamp of this history
     * @param serdesType  Type of node serializer/deserializer to use
     */
    void open(const boost::filesystem::path& path,
              std::size_t nodeSize = DEF_NODE_SIZE,
              std::size_t maxChildren = DEF_MAX_CHILDREN,
              timestamp_t begin = 0,
              NodeSerDesType serdesType = NodeSerDesType::ALIGNED);

    /**
     * @see IHistoryFileSink::close(timestamp_t)
     */
    void close(timestamp_t endTs);

    /**
     * Closes the history file using its current end timestamp. Use
     * close(timestamp_t) to specify another end timestamp.
     */
    void close();

    /**
     * @see IHistoryFileSink::addInterval(AbstractInterval::SP)
     */
    void addInterval(AbstractInterval::SP interval);

private:
    void writeHeader();
    void tryAddIntervalToNode(AbstractInterval::SP intr, std::size_t index);
    void incNodeCount(timestamp_t begin);
    void addSiblingNode(std::size_t index);
    void drawBranchFromIndex(std::size_t parentIndex,
                             std::size_t height);
    void commitNodesDownFromIndex(std::size_t index);
    void commitNode(Node& node);
    Node::UP createNode(node_seq_t parentSeqNumber, timestamp_t begin) const;

private:
    boost::filesystem::ofstream _outputStream;
    std::unique_ptr<std::uint8_t[]> _nodeBuf;
};

#endif // _HISTORYFILESINK_HPP
