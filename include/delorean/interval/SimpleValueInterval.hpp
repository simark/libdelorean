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
#include <delorean/interval/StandardIntervalTypes.hpp>
#include <delorean/BasicTypes.hpp>

template<typename T, StandardIntervalTypes SIT>
class SimpleValueInterval :
    public AbstractInterval
{
public:
    SimpleValueInterval(timestamp_t begin, timestamp_t end, interval_id_t id);

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
    std::size_t getVariableSizeImpl() const;
    void serializeValuesImpl(std::uint8_t* fixedPtr,
                             std::uint8_t* varAtPtr) const;
    void deserializeValuesImpl(const std::uint8_t* fixedPtr,
                               const std::uint8_t* varEndPtr);

private:
    T _value;
};

template<typename T, StandardIntervalTypes SIT>
SimpleValueInterval<T, SIT>::SimpleValueInterval(timestamp_t begin,
                                                 timestamp_t end,
                                                 interval_id_t id) :
    AbstractInterval {
        begin,
        end,
        id,
        SIT
    }
{
}
template<typename T, StandardIntervalTypes SIT>
std::size_t SimpleValueInterval<T, SIT>::getVariableSizeImpl() const
{
    return 0;
}

template<typename T, StandardIntervalTypes SIT>
void SimpleValueInterval<T, SIT>::serializeValuesImpl(std::uint8_t* fixedPtr,
                                                      std::uint8_t* varAtPtr) const
{
    std::memcpy(fixedPtr, &_value, sizeof(_value));
}

template<typename T, StandardIntervalTypes SIT>
void SimpleValueInterval<T, SIT>::deserializeValuesImpl(const std::uint8_t* fixedPtr,
                                                        const std::uint8_t* varEndPtr)
{
    std::memcpy(&_value, fixedPtr, sizeof(_value));
}

#endif // _SIMPLEVALUEINTERVAL_HPP
