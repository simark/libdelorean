/**
 * Copyright (c) 2012 Philippe Proulx <philippe.proulx@polymtl.ca>
 * Copyright (c) 2012 François Rajotte <francois.rajotte@polymtl.ca>
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
#include <iomanip>
#include <iostream>
#include <cstring>
#include <map>

#include "../basic_types.h"
#include "AbstractInterval.hpp"
#include "IntInterval.hpp"

using namespace std;

const unsigned int AbstractInterval::HEADER_SIZE = (
	2 * sizeof(timestamp_t) +	// start/end
	sizeof(attribute_t) +		// attribute
	sizeof(interval_type_t) +	// interval type
	sizeof(uint32_t)		// value/variable section pointer
);

AbstractInterval::AbstractInterval(timestamp_t start, timestamp_t end, attribute_t attribute, interval_type_t type)
: _start(start), _end(end), _attribute(attribute), _type(type)
{
}

std::string AbstractInterval::toString(void) const
{
	map<uint8_t, string> types;
	types.insert(make_pair(SIT_INT32, "int32"));
	types.insert(make_pair(SIT_UINT32, "uint32"));
	types.insert(make_pair(SIT_FLOAT32, "float"));
	types.insert(make_pair(SIT_STRING, "str"));
	
	ostringstream type_name;
	if (types.find(this->_type) == types.end()) {
		type_name << (unsigned int) this->_type;
	} else {
		type_name << types[this->_type];
	}
	
	ostringstream oss;
	oss << "@ " << this->_attribute << " (" << type_name.str() << ") " <<
		"[" << this->_start << ", " << this->_end << "] : " <<
		this->getStringValue();
		
	return oss.str();
}

bool AbstractInterval::intersects(timestamp_t ts) const {
	return this->_start <= ts && this->_end >= ts;
}

ostream& operator<<(ostream& out, AbstractInterval const& intr)
{
	out << intr.toString();
	
	return out;
}

void AbstractInterval::serialize(uint8_t* var_ptr, uint8_t* head_ptr) {
	// serialize header
	memcpy(head_ptr, &this->_start, sizeof(timestamp_t));
	head_ptr += sizeof(timestamp_t);
	memcpy(head_ptr, &this->_end, sizeof(timestamp_t));
	head_ptr += sizeof(timestamp_t);
	memcpy(head_ptr, &this->_attribute, sizeof(attribute_t));
	head_ptr += sizeof(attribute_t);
	memcpy(head_ptr, &this->_type, sizeof(interval_type_t));
	head_ptr += sizeof(interval_type_t);
	
	// serialize values
	this->serializeValues(var_ptr, head_ptr);
}

unsigned int AbstractInterval::unserialize(uint8_t* var_ptr, uint8_t* head_ptr) {
	// unserialize header
	memcpy(&this->_start, head_ptr, sizeof(timestamp_t));
	head_ptr += sizeof(timestamp_t);
	memcpy(&this->_end, head_ptr, sizeof(timestamp_t));
	head_ptr += sizeof(timestamp_t);
	memcpy(&this->_attribute, head_ptr, sizeof(attribute_t));
	head_ptr += sizeof(attribute_t);
	memcpy(&this->_type, head_ptr, sizeof(interval_type_t));
	head_ptr += sizeof(interval_type_t);
	
	// unserialize values
	return this->unserializeValues(var_ptr, head_ptr);
}

bool AbstractInterval::operator==(const AbstractInterval& other)
{
	return this->_end == other._end;
}

bool AbstractInterval::operator<(const AbstractInterval& other)
{
	return this->_end < other._end;
}

bool AbstractInterval::operator<=(const AbstractInterval& other)
{
	return this->_end <= other._end;
}

bool AbstractInterval::operator!=(const AbstractInterval& other)
{
	return this->_end != other._end;
}

bool AbstractInterval::operator>(const AbstractInterval& other)
{
	return this->_end > other._end;
}

bool AbstractInterval::operator>=(const AbstractInterval& other)
{
	return this->_end >= other._end;
}
