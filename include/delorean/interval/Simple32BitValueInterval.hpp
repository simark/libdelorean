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
#ifndef _SIMPLE32BITVALUEINTERVAL_HPP
#define _SIMPLE32BITVALUEINTERVAL_HPP

#include <memory>
#include <cstdint>
#include <cstddef>
#include <cstring>

#include <delorean/interval/AbstractInterval.hpp>
#include <delorean/interval/StandardIntervalType.hpp>
#include <delorean/BasicTypes.hpp>

namespace delo
{

/**
 * Template class for creating an interval class with a simple 32-bit type
 * for its value.
 *
 * @author Philippe Proulx
 */
template<typename T, StandardIntervalType SIT>
class Simple32BitValueInterval :
    public AbstractInterval
{
public:
    Simple32BitValueInterval(timestamp_t begin, timestamp_t end,
                            interval_key_t key);

    virtual ~Simple32BitValueInterval()
    {
    }

    void setValue(const T value)
    {
        this->setFixedValue(static_cast<interval_value_t>(value));
    }

    T getValue() const
    {
        return static_cast<T>(this->getFixedValue());
    }

protected:
    std::size_t getVariableDataSizeImpl() const;
    void serializeVariableDataImpl(std::uint8_t* varAtPtr) const;
    void deserializeVariableDataImpl(const std::uint8_t* varAtPtr);
};

template<typename T, StandardIntervalType SIT>
Simple32BitValueInterval<T, SIT>::Simple32BitValueInterval(timestamp_t begin,
                                                           timestamp_t end,
                                                           interval_key_t key) :
    AbstractInterval {
        begin,
        end,
        key,
        SIT
    }
{
}

template<typename T, StandardIntervalType SIT>
std::size_t Simple32BitValueInterval<T, SIT>::getVariableDataSizeImpl() const
{
    return 0;
}

template<typename T, StandardIntervalType SIT>
void Simple32BitValueInterval<T, SIT>::serializeVariableDataImpl(std::uint8_t* varAtPtr) const
{
    // no variable data
}

template<typename T, StandardIntervalType SIT>
void Simple32BitValueInterval<T, SIT>::deserializeVariableDataImpl(const std::uint8_t* varAtPtr)
{
    // no variable data
}

}

#endif // _SIMPLE32BITVALUEINTERVAL_HPP
