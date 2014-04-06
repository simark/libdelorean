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
#include <sstream>
#include <cstring>

#include <delorean/intervals/StringInterval.hpp>
#include <delorean/basic_types.h>
#include <delorean/ex/IOEx.hpp>

using namespace std;

StringInterval::StringInterval(timestamp_t start, timestamp_t end, attribute_t attribute, string value)
: AbstractInterval(start, end, attribute, SIT_STRING), _value(value)
{
}

StringInterval::StringInterval(timestamp_t start, timestamp_t end, attribute_t attribute, const char* value)
: AbstractInterval(start, end, attribute, SIT_STRING), _value(value)
{
}

std::string StringInterval::getStringValue(void) const
{
	return _value;
}

void StringInterval::serializeValues(uint8_t* var_addr, uint8_t* u32_addr) const {
	// copy string length (enables faster reads than ASCIIZ)
	uint8_t sz = _value.size()+2;
	memcpy(var_addr, &sz, sizeof(sz));
		
	// copy string
	memcpy((unsigned char*) var_addr + sizeof(sz), _value.c_str(), _value.size());
	
	// add NULL character
	*(var_addr+sz-1) = 0;
}

unsigned int StringInterval::unserializeValues(uint8_t* var_addr, uint8_t* u32_addr) {
	// read string length
	uint8_t sz;
	memcpy(&sz, var_addr, sizeof(sz));
	
	// check NULL character
	if(*(var_addr+sz-1) != 0) {
		throw IOEx("Corrupt string entry");
	}
	
	// read string
	_value.assign((char*) var_addr + sizeof(sz), sz-2);
	
	return sz;
}

unsigned int StringInterval::getVariableValueSize(void) const {
	// length of string + 8-bit header + 8-bit NULL character
	return sizeof(uint8_t)*2 + _value.size();
}

AbstractInterval::SharedPtr StringInterval::clone(void) const{
	return AbstractInterval::SharedPtr(new StringInterval(*this));
}
