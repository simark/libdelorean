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
#include <delorean/intervals/FloatInterval.hpp>
#include <delorean/basic_types.h>

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

AbstractInterval::SharedPtr FloatInterval::clone(void) const{
	return AbstractInterval::SharedPtr(new FloatInterval(*this));
}
