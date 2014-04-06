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
#ifndef _UINTINTERVAL_HPP
#define _UINTINTERVAL_HPP

#include <stdint.h>

#include "AbstractInterval.hpp"
#include "../BasicTypes.hpp"
#include "../FixedConfig.hpp"

class UIntInterval : public AbstractInterval
{
public:
    typedef std::tr1::shared_ptr<UIntInterval> SharedPtr;

    UIntInterval(void) : AbstractInterval(SIT_UINT32) { }
    UIntInterval(timestamp_t start, timestamp_t end, attribute_t attribute, uint32_t value);
    ~UIntInterval() { }
    std::string getStringValue(void) const;
    unsigned int getVariableValueSize(void) const;
    AbstractInterval::SharedPtr clone(void) const;
    void setValue(uint32_t value) {_value = value;}
    uint32_t getValue() const {return _value;}

    static const interval_type_t type = SIT_UINT32;

protected:
    void serializeValues(uint8_t* var_addr, uint8_t* u32_addr) const;
    unsigned int unserializeValues(uint8_t* var_addr, uint8_t* u32_addr);

private:
    uint32_t _value;
};

#endif // _UINTINTERVAL_HPP
