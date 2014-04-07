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
#include <delorean/AbstractHistorySource.hpp>
#include <delorean/interval/IntervalJar.hpp>
#include <delorean/interval/AbstractInterval.hpp>
#include <delorean/BasicTypes.hpp>

class HistoryFileSource :
    public AbstractFile,
    public AbstractHistorySource
{
public:
    virtual ~HistoryFileSource()
    {
        this->close();
    }

    void open(const boost::filesystem::path& path);
    void close();

    bool query(timestamp_t ts, IntervalJar& intervals) const;
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
