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
#ifndef _TIMESTAMPOUTOFRANGE_HPP
#define _TIMESTAMPOUTOFRANGE_HPP

#include <stdexcept>

#include <delorean/BasicTypes.hpp>

class TimestampOutOfRange :
    public std::out_of_range
{
public:
    TimestampOutOfRange(timestamp_t begin, timestamp_t end, timestamp_t ts) :
        std::out_of_range {"Timestamp out of range"},
        _begin {begin},
        _end {end},
        _ts {ts}
    {
    }

    timestamp_t getBegin() const
    {
        return _begin;
    }

    timestamp_t getEnd() const
    {
        return _end;
    }

    timestamp_t getTs() const
    {
        return _ts;
    }

private:
    timestamp_t _begin;
    timestamp_t _end;
    timestamp_t _ts;
};

#endif // _TIMESTAMPOUTOFRANGE_HPP
