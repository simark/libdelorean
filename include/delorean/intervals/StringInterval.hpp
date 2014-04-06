/**
 * Copyright (c) 2012 Philippe Proulx <philippe.proulx@polymtl.ca>
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

#include <string>

#include "AbstractInterval.hpp"
#include "../BasicTypes.hpp"
#include "../FixedConfig.hpp"

class StringInterval : public AbstractInterval
{
public:
    typedef std::tr1::shared_ptr<StringInterval> SharedPtr;

    StringInterval(void) : AbstractInterval(SIT_STRING) { }
    StringInterval(timestamp_t start, timestamp_t end, attribute_t attribute, std::string value);
    StringInterval(timestamp_t start, timestamp_t end, attribute_t attribute, const char* value);
    std::string getStringValue(void) const;
    unsigned int getVariableValueSize(void) const;
    AbstractInterval::SharedPtr clone(void) const;
    void setValue(std::string value) {_value = value;}
    std::string getValue() const {return _value;}

    static const interval_type_t type = SIT_STRING;

protected:
    void serializeValues(uint8_t* var_addr, uint8_t* u32_addr) const;
    unsigned int unserializeValues(uint8_t* var_addr, uint8_t* u32_addr);

private:
    std::string _value;
};

#endif // _STRINGINTERVAL_HPP
