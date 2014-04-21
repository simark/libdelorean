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
#ifndef _INVALIDINTERVALARGUMENTS_HPP
#define _INVALIDINTERVALARGUMENTS_HPP

#include <stdexcept>

#include <delorean/BasicTypes.hpp>

class InvalidIntervalArguments :
    public std::invalid_argument
{
public:
    InvalidIntervalArguments(timestamp_t begin, timestamp_t end) :
        std::invalid_argument {"Invalid interval arguments when constructing"},
        _begin {begin},
        _end {end}
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

private:
    timestamp_t _begin;
    timestamp_t _end;
};

#endif // _INVALIDINTERVALARGUMENTS_HPP
