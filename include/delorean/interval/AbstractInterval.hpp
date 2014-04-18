/**
 * Copyright (c) 2014 Philippe Proulx <philippe.proulx@polymtl.ca>
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
#ifndef _ABSTRACTINTERVAL_HPP
#define _ABSTRACTINTERVAL_HPP

#include <string>
#include <memory>
#include <iostream>
#include <cstdint>
#include <cstddef>
#include <boost/operators.hpp>

#include <delorean/BasicTypes.hpp>
#include <delorean/interval/StandardIntervalType.hpp>

class AbstractInterval :
    boost::totally_ordered<AbstractInterval>
{
public:
    typedef std::shared_ptr<AbstractInterval> SP;
    typedef std::unique_ptr<AbstractInterval> UP;
    typedef std::shared_ptr<const AbstractInterval> CSP;
    typedef std::unique_ptr<const AbstractInterval> CUP;

public:
    AbstractInterval(timestamp_t begin, timestamp_t end,
                     interval_id_t id, interval_type_t type);
    AbstractInterval(timestamp_t begin, timestamp_t end,
                     interval_id_t id, StandardIntervalType type);
    virtual ~AbstractInterval();

    std::size_t getVariableDataSize() const;
    void serializeVariableData(std::uint8_t* varAtPtr) const;
    void deserializeVariableData(const std::uint8_t* varAtPtr);

    void setFixedValue(interval_value_t value);
    interval_value_t getFixedValue() const;

    bool intersects(timestamp_t ts) const
    {
        return ts >= _begin && ts <= _end;
    }

    timestamp_t getBegin() const
    {
        return _begin;
    }

    timestamp_t getEnd() const
    {
        return _end;
    }

    interval_id_t getId() const
    {
        return _id;
    }

    interval_type_t getType() const
    {
        return _type;
    }

    bool operator==(const AbstractInterval& interval) const
    {
        return interval._end == _end;
    }

    bool operator<(const AbstractInterval& interval) const
    {
        return _end < interval._end;
    }

private:
    virtual std::size_t getVariableDataSizeImpl() const = 0;
    virtual void serializeVariableDataImpl(std::uint8_t* varAtPtr) const = 0;
    virtual void deserializeVariableDataImpl(const std::uint8_t* varAtPtr) = 0;
    virtual void setFixedValueImpl(interval_value_t value) = 0;
    virtual interval_value_t getFixedValueImpl() const = 0;

private:
    // interval
    timestamp_t _begin;
    timestamp_t _end;

    // type
    interval_type_t _type;

    // ID
    interval_id_t _id;
};

#endif // _ABSTRACTINTERVAL_HPP
