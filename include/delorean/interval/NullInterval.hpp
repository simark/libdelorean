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
#ifndef _NULLINTERVAL_HPP
#define _NULLINTERVAL_HPP

#include <memory>

#include <delorean/interval/AbstractInterval.hpp>
#include <delorean/interval/StandardIntervalTypes.hpp>
#include <delorean/BasicTypes.hpp>

class NullInterval :
    public AbstractInterval
{
public:
    typedef std::shared_ptr<NullInterval> SP;
    typedef std::unique_ptr<NullInterval> UP;

public:
    NullInterval(timestamp_t begin, timestamp_t end, interval_id_t id) :
        AbstractInterval {
            begin,
            end,
            id,
            StandardIntervalTypes::TNULL
        }
    {
    }

    virtual ~NullInterval()
    {
    }

private:
    std::size_t getVariableSizeImpl() const
    {
        return 0;
    }

    void serializeValuesImpl(std::uint8_t* fixedPtr,
                             std::uint8_t* varAtPtr) const
    {
    }

    void deserializeValuesImpl(const std::uint8_t* fixedPtr,
                               const std::uint8_t* varEndPtr)
    {
    }
};

#endif // _NULLINTERVAL_HPP
