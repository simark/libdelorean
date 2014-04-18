/**
 * Copyright (c) 2014 Philippe Proulx <eepp.ca>
 * Copyright (c) 2012 François Rajotte <francois.rajotte@polymtl.ca>
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
#include <cstddef>
#include <cstring>
#include <cstdint>

#include <delorean/BasicTypes.hpp>
#include <delorean/interval/AbstractInterval.hpp>
#include <delorean/interval/StandardIntervalType.hpp>
#include <delorean/ex/InvalidIntervalArguments.hpp>

AbstractInterval::AbstractInterval(timestamp_t begin, timestamp_t end,
                                   interval_id_t id,
                                   interval_type_t type) :
    _begin {begin},
    _end {end},
    _type {type},
    _id {id}
{
    // check range
    if (begin > end) {
        throw InvalidIntervalArguments(begin, end);
    }
}

AbstractInterval::AbstractInterval(timestamp_t begin, timestamp_t end,
                                   interval_id_t id,
                                   StandardIntervalType type) :
    AbstractInterval {
        begin,
        end,
        id,
        static_cast<interval_type_t>(type)
    }
{
}

AbstractInterval::~AbstractInterval()
{
}

std::size_t AbstractInterval::getVariableDataSize() const
{
    return this->getVariableDataSizeImpl();
}

void AbstractInterval::serializeVariableData(std::uint8_t* varAtPtr) const
{
    this->serializeVariableDataImpl(varAtPtr);
}

void AbstractInterval::deserializeVariableData(const std::uint8_t* varAtPtr)
{
    this->deserializeVariableDataImpl(varAtPtr);
}

void AbstractInterval::setFixedValue(interval_value_t value)
{
    this->setFixedValueImpl(value);
}

interval_value_t AbstractInterval::getFixedValue() const
{
    return this->getFixedValueImpl();
}
