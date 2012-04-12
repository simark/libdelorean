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
#include <rbrntrvll/intervals/FloatInterval.hpp>
#include <rbrntrvll/basic_types.h>

#include <sstream>

FloatInterval::FloatInterval(timestamp_t start, timestamp_t end, attribute_t attribute, float value)
:AbstractInterval(start, end, attribute, SIT_FLOAT32), _value(value)
{
}

std::string FloatInterval::getStringValue(void) const
{
	std::ostringstream oss;
	oss << _value;
	return oss.str();
}

void FloatInterval::serializeValues(uint8_t* var_addr, uint8_t* u32_addr) const {
	*((float*) u32_addr) = this->_value;
}

unsigned int FloatInterval::unserializeValues(uint8_t* var_addr, uint8_t* u32_addr) {
	this->_value = *((float*) u32_addr);
	
	return 0;
}

unsigned int FloatInterval::getVariableValueSize(void) const {
	return 0;
}

AbstractInterval* FloatInterval::clone(void) const{
	return new FloatInterval(*this);
}