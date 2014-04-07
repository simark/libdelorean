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
#ifndef _INTERVALSERIALIZER_HPP
#define _INTERVALSERIALIZER_HPP

#include <vector>
#include <cstdint>

#include <delorean/interval/AbstractInterval.hpp>
#include <delorean/BasicTypes.hpp>

class IntervalSerializer
{
public:
    IntervalSerializer()
    {
    }

    ~IntervalSerializer()
    {
    }

    void serializeInterval(const AbstractInterval& interval,
                           std::uint8_t* headPtr,
                           std::uint8_t* varAtPtr,
                           std::size_t varOffset) const;
};

#endif // _INTERVALSERIALIZER_HPP
