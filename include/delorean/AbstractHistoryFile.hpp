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
#ifndef _ABSTRACTHISTORYFILE_HPP
#define _ABSTRACTHISTORYFILE_HPP

#include <boost/filesystem.hpp>
#include <boost/utility.hpp>

#include <delorean/BasicTypes.hpp>

/**
 * Base class for all history files, that is, HistoryFileSink and
 * HistoryFileSource.
 *
 * @see HistoryFileSink
 * @see HistoryFileSource
 * @author Philippe Proulx
 */
class AbstractHistoryFile :
    boost::noncopyable
{
public:
    AbstractHistoryFile() :
        _isOpened {false}
    {
    }

    virtual ~AbstractHistoryFile() = 0;

protected:
    struct HistoryFileHeader
    {
        enum {
            MAGIC = 0x21b4a980,
            SIZE = 4096,
            MAJOR = 1,
            MINOR = 0
        };

        uint32_t magic = MAGIC;
        uint16_t major = MAJOR;
        uint16_t minor = MINOR;
        uint32_t nodeSize;
        uint32_t maxChildren;
        uint32_t nodeCount;
        node_seq_t rootNodeSeqNumber;
    };

protected:
    const boost::filesystem::path& getPath() const
    {
        return _path;
    }

    void setPath(const boost::filesystem::path& path)
    {
        _path = path;
    }

    void setOpened(bool isOpened)
    {
        _isOpened = true;
    }

    bool isOpened() const
    {
        return _isOpened;
    }

private:
    bool _isOpened;
    boost::filesystem::path _path;

};

#endif // _ABSTRACTHISTORYFILE_HPP
