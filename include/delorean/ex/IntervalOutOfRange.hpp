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
#ifndef _INTERVALOUTOFRANGE_HPP
#define _INTERVALOUTOFRANGE_HPP

#include <stdexcept>

#include <delorean/interval/AbstractInterval.hpp>
#include <delorean/BasicTypes.hpp>

namespace delo
{
namespace ex
{

class IntervalOutOfRange :
    public std::out_of_range
{
public:
    IntervalOutOfRange(const AbstractInterval& interval,
                       timestamp_t rangeBegin, timestamp_t rangeEnd) :
        std::out_of_range {"Timestamp out of range"},
        _rangeBegin {rangeBegin},
        _rangeEnd {rangeEnd}
    {
        _intervalBegin = interval.getBegin();
        _intervalEnd = interval.getEnd();
    }

    timestamp_t getIntervalBegin() const
    {
        return _intervalBegin;
    }

    timestamp_t getIntervalEnd() const
    {
        return _intervalEnd;
    }

    timestamp_t getRangeBegin() const
    {
        return _rangeBegin;
    }

    timestamp_t getRangeEnd() const
    {
        return _rangeEnd;
    }

private:
    timestamp_t _rangeBegin;
    timestamp_t _rangeEnd;
    timestamp_t _intervalBegin;
    timestamp_t _intervalEnd;
};

}
}

#endif // _INTERVALOUTOFRANGE_HPP
