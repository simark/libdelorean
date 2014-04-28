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
#ifndef _HISTORYFILESOURCE_HPP
#define _HISTORYFILESOURCE_HPP

#include <vector>
#include <fstream>
#include <boost/filesystem/fstream.hpp>

#include <delorean/AbstractHistoryFile.hpp>
#include <delorean/IHistorySource.hpp>
#include <delorean/node/AbstractNodeCache.hpp>
#include <delorean/interval/IntervalJar.hpp>
#include <delorean/interval/AbstractInterval.hpp>
#include <delorean/BasicTypes.hpp>

/**
 * History file opened for input. Use an HistoryFileSource object to read and
 * find intervals within a history file.
 *
 * @see HistoryFileSink
 * @author Philippe Proulx
 */
class HistoryFileSource :
    public AbstractHistoryFile,
    public IHistorySource
{
public:
    /**
     * Builds a history file source. The file is initially closed and needs
     * to be opened with a valid path in order to start querying the
     * history.
     */
    HistoryFileSource();

    virtual ~HistoryFileSource()
    {
        this->close();
    }

    /**
     * Opens the history file for reading.
     *
     * @param path      Path to history file to read
     * @param nodeCache Specific node cache to use or \a nullptr for no cache
     */
    void open(const boost::filesystem::path& path,
              std::shared_ptr<AbstractNodeCache> nodeCache = nullptr);

    /**
     * Closes the history file.
     */
    void close();

    /**
     * @see IHistorySource::findAll(timestamp_t, IntervalJar&)
     */
    bool findAll(timestamp_t ts, IntervalJar& intervals);

    /**
     * @see IHistorySource::findOne(timestamp_t, interval_key_t)
     */
    AbstractInterval::SP findOne(timestamp_t ts, interval_key_t key);

protected:
    void readHeader();
    Node::SP getNode(node_seq_t seqNumber);
    Node::SP getNodeFromCache(node_seq_t seqNumber);
    Node::SP getRootNode();

private:
    boost::filesystem::ifstream _inputStream;
    std::unique_ptr<uint8_t[]> _nodeBuf;
    std::shared_ptr<AbstractNodeCache> _nodeCache;
};

#endif // _HISTORYFILESOURCE_HPP
