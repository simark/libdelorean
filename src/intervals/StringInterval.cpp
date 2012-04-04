/**
 * Copyright (c) 2012 Philippe Proulx <philippe.proulx@polymtl.ca>
 *
 * This file is part of librbntrvll.
 *
 * librbntrvll is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * librbntrvll is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with librbntrvll.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <sstream>
#include <cstring>

#include "StringInterval.hpp"
#include "../basic_types.h"

using namespace std;

StringInterval::StringInterval(timestamp_t start, timestamp_t end, attribute_t attribute, string value)
: Interval(start, end, attribute, SIT_STRING), _value(value)
{
}

StringInterval::StringInterval(timestamp_t start, timestamp_t end, attribute_t attribute, const char* value)
: Interval(start, end, attribute, SIT_STRING), _value(value)
{
}

std::string StringInterval::getStringValue(void) const
{
	return _value;
}

void StringInterval::serializeValues(uint8_t* var_addr, uint8_t* u32_addr) const {
	// copy string length (enables faster reads than ASCIIZ)
	uint32_t sz = _value.size();
	memcpy(var_addr, &sz, sizeof(sz));
	
	// copy string
	memcpy((unsigned char*) var_addr + sizeof(sz), _value.c_str(), _value.size());
}

unsigned int StringInterval::unserializeValues(uint8_t* var_addr, uint8_t* u32_addr) {
	// read string length
	uint32_t sz;
	memcpy(&sz, var_addr, sizeof(sz));
	
	// read string
	_value.assign((char*) var_addr + sizeof(sz), sz);
	
	return sizeof(sz) + sz;
}

unsigned int StringInterval::getVariableValueSize(void) const {
	// length of string + 32-bit header
	return sizeof(uint32_t) + _value.size();
}

Interval* StringInterval::clone(void) const{
	return new StringInterval(*this);
}
