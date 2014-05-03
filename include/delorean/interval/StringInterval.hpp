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
#ifndef _STRINGINTERVAL_HPP
#define _STRINGINTERVAL_HPP

#include <memory>
#include <cstdint>
#include <cstddef>

#include <delorean/interval/AbstractInterval.hpp>
#include <delorean/BasicTypes.hpp>

namespace delo
{

/**
 * Interval containing a string value. Variable data is serialized with a
 * terminating NUL character internally, so UTF-8 is allowed.
 *
 * @author Philippe Proulx
 */
class StringInterval :
    public AbstractInterval
{
public:
    typedef std::shared_ptr<StringInterval> SP;
    typedef std::unique_ptr<StringInterval> UP;

public:
    StringInterval(timestamp_t begin, timestamp_t end,
                   interval_key_t key);

    virtual ~StringInterval()
    {
    }

    void setValue(const std::string& value)
    {
        _value = value;
    }

    const std::string& getValue() const
    {
        return _value;
    }

protected:
    std::size_t getVariableDataSizeImpl() const;
    void serializeVariableDataImpl(std::uint8_t* varAtPtr) const;
    void deserializeVariableDataImpl(const std::uint8_t* varAtPtr);

private:
    std::string _value;
};

}

#endif // _STRINGINTERVAL_HPP
