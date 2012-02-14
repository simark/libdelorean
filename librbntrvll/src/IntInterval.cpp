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
#include "IntInterval.hpp"

#include <sstream>

IntInterval::IntInterval(uint64_t start, uint64_t end, uint32_t attribute, int32_t value)
:Interval(start, end, attribute), _value(value)
{
}

std::string IntInterval::getStringValue(void) const
{
	std::ostringstream oss;
	oss << _value;
	return oss.str();
}

void IntInterval::serialize(void* var_addr, void* u32_addr) const {
	*((int32_t*) u32_addr) = this->_value;
}

void IntInterval::unserialize(void* var_addr, void* u32_addr) {
	this->_value = *((int32_t*) u32_addr);
}

unsigned int IntInterval::getVariableValueSize(void) const {
	return 0;
}
