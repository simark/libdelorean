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
#include <cstddef>
#include <cstring>

#include <delorean/interval/StringInterval.hpp>
#include <delorean/interval/StandardIntervalType.hpp>
#include <delorean/BasicTypes.hpp>

namespace delo
{

StringInterval::StringInterval(timestamp_t begin, timestamp_t end,
                               interval_key_t key) :
    AbstractInterval {
        begin,
        end,
        key,
        StandardIntervalType::STRING
    }
{
}

std::size_t StringInterval::getVariableDataSizeImpl() const
{
    // includes NUL character
    return _value.size() + 1;
}

void StringInterval::serializeVariableDataImpl(std::uint8_t* varAtPtr) const
{
    // write string to variable section
    std::memcpy(varAtPtr, _value.c_str(), this->getVariableDataSizeImpl());
}

void StringInterval::deserializeVariableDataImpl(const std::uint8_t* varAtPtr)
{
    // build string (safe since pointed data is NUL-terminated)
    const char* cstr = reinterpret_cast<const char*>(varAtPtr);
    _value = std::string(cstr);
}

}
