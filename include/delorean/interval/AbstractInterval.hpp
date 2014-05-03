/* Copyright (c) 2014 Philippe Proulx <eepp.ca>
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

namespace delo
{

/**
 * Base of all intervals.
 *
 * All intervals to be used with libdelorean must inherit this abstract
 * class.
 *
 * All intervals are totally ordered by their end timestamp using typical
 * comparison operators.
 *
 * @author Philippe Proulx
 */
class AbstractInterval :
    boost::totally_ordered<AbstractInterval>
{
public:
    /// Shared pointer to abstract interval
    typedef std::shared_ptr<AbstractInterval> SP;

    /// Unique pointer to abstract interval
    typedef std::unique_ptr<AbstractInterval> UP;

public:
    /**
     * Builds an abstract interval with a custom type.
     *
     * @param begin Begin timestamp
     * @param end   End timestamp (excluded)
     * @param key   Key
     * @param type  Type
     */
    AbstractInterval(timestamp_t begin, timestamp_t end,
                     interval_key_t key, interval_type_t type);

    /**
     * Builds an abstract interval with a standard type.
     *
     * @param begin Begin timestamp
     * @param end   End timestamp (excluded)
     * @param key   Key
     * @param type  Standard type
     */
    AbstractInterval(timestamp_t begin, timestamp_t end,
                     interval_key_t key, StandardIntervalType type);

    virtual ~AbstractInterval();

    /**
     * Returns the size of this interval's variable data in bytes. A return
     * value of 0 means this interval has no variable data.
     *
     * @returns Size of variable data in bytes
     */
    std::size_t getVariableDataSize() const;

    /**
     * Serializes this interval's variable data at address \p varAtPtr.
     * Caller must ensure there's at least the size returned by
     * getVariableDataSize() at \p varAtPtr.
     *
     * @param varAtPtr Address at which to write variable data
     */
    void serializeVariableData(std::uint8_t* varAtPtr) const;

    /**
     * Deserializes variable data at address \p varAtPtr into this interval.
     *
     * @param varAtPtr Address at which to read variable data
     */
    void deserializeVariableData(const std::uint8_t* varAtPtr);

    /**
     * Sets the fixed 32-bit value of this interval. Please note it's useless
     * to call this method if the interval has to contain any variable data.
     *
     * @param fixedValue Fixed 32-bit value to set
     */
    void setFixedValue(interval_value_t fixedValue);

    /**
     * Returns the fixed 32-bit value of this interval. Please note this
     * value is meaningless if the interval contains any variable data.
     *
     * @returns Fixed 32-bit value of this interval
     */
    interval_value_t getFixedValue() const;

    /**
     * Returns whether timestamp \p ts intersects with this interval or not.
     *
     * @param ts Timestamp to check
     * @returns  True if timestamp \p ts intersects with this interval
     */
    bool intersects(timestamp_t ts) const
    {
        return ts >= _begin && ts < _end;
    }

    /**
     * Returns the begin timestamp.
     *
     * @returns Begin timestamp
     */
    timestamp_t getBegin() const
    {
        return _begin;
    }

    /**
     * Returns the end timestamp (excluded from the interval).
     *
     * @returns End timestamp
     */
    timestamp_t getEnd() const
    {
        return _end;
    }

    /**
     * Returns the key.
     *
     * @returns Key
     */
    interval_key_t getKey() const
    {
        return _key;
    }

    /**
     * Returns the type.
     *
     * @returns Interval type
     */
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
    /**
     * Virtual implementation of getVariableDataSize(); must be implemented by
     * child class.
     */
    virtual std::size_t getVariableDataSizeImpl() const = 0;

    /**
     * Virtual implementation of serializeVariableData(); must be implemented by
     * child class.
     */
    virtual void serializeVariableDataImpl(std::uint8_t* varAtPtr) const = 0;

    /**
     * Virtual implementation of deserializeVariableData(); must be implemented by
     * child class.
     */
    virtual void deserializeVariableDataImpl(const std::uint8_t* varAtPtr) = 0;

private:
    // interval
    timestamp_t _begin;
    timestamp_t _end;

    // type
    interval_type_t _type;

    // key
    interval_key_t _key;

    // 32-bit value
    interval_value_t _fixedValue;
};

}

#endif // _ABSTRACTINTERVAL_HPP
