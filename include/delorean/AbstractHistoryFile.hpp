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
#ifndef _ABSTRACTHISTORYFILE_HPP
#define _ABSTRACTHISTORYFILE_HPP

#include <boost/filesystem.hpp>

#include <delorean/BasicTypes.hpp>

class AbstractHistoryFile
{
public:
    AbstractHistoryFile() :
        _isOpened {false}
    {
    }

    virtual ~AbstractHistoryFile() = 0;

protected:
    struct HistoryFileHeader {
        enum {
            MAGIC = 0x05eea901,
            SIZE = 4096,
            MAJOR = 1,
            MINOR = 0
        };

        uint32_t magic = MAGIC;
        uint32_t major = MAJOR;
        uint32_t minor = MINOR;
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
