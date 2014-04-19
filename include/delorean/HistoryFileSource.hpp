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
#ifndef _HISTORYFILESOURCE_HPP
#define _HISTORYFILESOURCE_HPP

#include <vector>
#include <fstream>
#include <boost/filesystem.hpp>

#include <delorean/AbstractFile.hpp>
#include <delorean/IHistorySource.hpp>
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
    public AbstractFile,
    public IHistorySource
{
public:
    virtual ~HistoryFileSource()
    {
        this->close();
    }

    /**
     * Opens the history file for reading.
     *
     * @param path        Path to history file to be created
     */
    void open(const boost::filesystem::path& path);

    /**
     * Closes the history file.
     */
    void close();

    /**
     * @see IHistorySource::query(timestamp_t, IntervalJar&)
     */
    bool query(timestamp_t ts, IntervalJar& intervals) const;

    /**
     * @see IHistorySource::queryFirstMatching(timestamp_t, interval_id_t)
     */
    AbstractInterval::SP queryFirstMatching(timestamp_t ts,
                                            interval_id_t id) const;

protected:
    bool queryImpl(timestamp_t ts, IntervalJar& intervals) const;
    AbstractInterval::SP queryFirstMatchingImpl(timestamp_t ts,
                                                interval_id_t id) const;

private:
    boost::filesystem::ifstream _inputStream;
    std::unique_ptr<uint8_t[]> _nodeBuf;
};

#endif // _HISTORYFILESOURCE_HPP
