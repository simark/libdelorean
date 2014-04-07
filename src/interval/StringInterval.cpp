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
#include <cstddef>
#include <cstring>

#include <delorean/interval/StringInterval.hpp>
#include <delorean/BasicTypes.hpp>

StringInterval::StringInterval(timestamp_t begin, timestamp_t end,
                               interval_id_t id) :
    AbstractInterval {
        begin,
        end,
        id,
        StandardIntervalTypes::STRING
    }
{
}

std::size_t StringInterval::getVariableSizeImpl() const
{
    // includes NUL character
    return _value.size() + 1;
}

void StringInterval::serializeValuesImpl(std::uint8_t* fixedPtr,
                                         std::uint8_t* varAtPtr) const
{
    // write string to variable section
    std::memcpy(varAtPtr, _value.c_str(), this->getVariableSize());
}

void StringInterval::deserializeValuesImpl(const std::uint8_t* fixedPtr,
                                           const std::uint8_t* varEndPtr)
{
    // read variable data offset
    interval_value_t offset;
    std::memcpy(&offset, fixedPtr, sizeof(offset));

    // point the beginning of the string
    auto cstr = varEndPtr - offset;

    // build string (pointed data is NUL-terminated)
    _value = std::string(reinterpret_cast<const char*>(cstr));
}
