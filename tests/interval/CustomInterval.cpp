/**
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
#include <cstring>

#include "CustomInterval.hpp"
#include "basic_types.h"

#include <ostream>

using namespace std;

std::string CustomInterval::getStringValue(void) const
{
	ostringstream oss;
	oss << _value.x << " " << _value.y;
	return oss.str();
}

void CustomInterval::serializeValues(uint8_t* var_addr, uint8_t* u32_addr) const {	
	// copy data
	memcpy((unsigned char*) var_addr, &_value.x, sizeof(double));
	memcpy((unsigned char*) var_addr + sizeof(double), &_value.y, sizeof(double));
}

unsigned int CustomInterval::unserializeValues(uint8_t* var_addr, uint8_t* u32_addr) {	
	// read string
	_value.x = *(double*)var_addr;
	_value.y = *(double*)(var_addr+sizeof(double));
	
	return sizeof(double)*2;
}

unsigned int CustomInterval::getVariableValueSize(void) const {
	// length of struct
	return sizeof(data);
}

AbstractInterval::SharedPtr CustomInterval::clone() const{
	return AbstractInterval::SharedPtr(new CustomInterval(*this));
}
