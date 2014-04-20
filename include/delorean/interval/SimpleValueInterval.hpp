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
#ifndef _SIMPLEVALUEINTERVAL_HPP
#define _SIMPLEVALUEINTERVAL_HPP

#include <memory>
#include <cstdint>
#include <cstddef>
#include <cstring>

#include <delorean/interval/AbstractInterval.hpp>
#include <delorean/interval/StandardIntervalType.hpp>
#include <delorean/BasicTypes.hpp>

/**
 * Template class for creating an interval class with a simple 32-bit type
 * for its value.
 *
 * @author Philippe Proulx
 */
template<typename T, StandardIntervalType SIT>
class SimpleValueInterval :
    public AbstractInterval
{
public:
    SimpleValueInterval(timestamp_t begin, timestamp_t end,
                        interval_cat_id_t catId);

    virtual ~SimpleValueInterval()
    {
    }

    void setValue(const T& value)
    {
        _value = value;
    }

    T getValue() const
    {
        return _value;
    }

private:
    std::size_t getVariableDataSizeImpl() const;
    void serializeVariableDataImpl(std::uint8_t* varAtPtr) const;
    void deserializeVariableDataImpl(const std::uint8_t* varAtPtr);
    void setFixedValueImpl(interval_value_t value);
    interval_value_t getFixedValueImpl() const;

private:
    T _value;
};

template<typename T, StandardIntervalType SIT>
SimpleValueInterval<T, SIT>::SimpleValueInterval(timestamp_t begin,
                                                 timestamp_t end,
                                                 interval_cat_id_t catId) :
    AbstractInterval {
        begin,
        end,
        catId,
        SIT
    }
{
}

template<typename T, StandardIntervalType SIT>
std::size_t SimpleValueInterval<T, SIT>::getVariableDataSizeImpl() const
{
    return 0;
}

template<typename T, StandardIntervalType SIT>
void SimpleValueInterval<T, SIT>::serializeVariableDataImpl(std::uint8_t* varAtPtr) const
{
    // no variable data
}

template<typename T, StandardIntervalType SIT>
void SimpleValueInterval<T, SIT>::deserializeVariableDataImpl(const std::uint8_t* varAtPtr)
{
    // no variable data
}

template<typename T, StandardIntervalType SIT>
void SimpleValueInterval<T, SIT>::setFixedValueImpl(interval_value_t value)
{
    _value = static_cast<T>(value);
}

template<typename T, StandardIntervalType SIT>
interval_value_t SimpleValueInterval<T, SIT>::getFixedValueImpl() const
{
    return static_cast<interval_value_t>(_value);
}

#endif // _SIMPLEVALUEINTERVAL_HPP
