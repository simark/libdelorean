/**
 * Copyright (c) 2012 Philippe Proulx <philippe.proulx@polymtl.ca>
 * Copyright (c) 2012 François Rajotte <francois.rajotte@polymtl.ca>
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
#include <delorean/intervals/NullInterval.hpp>
#include <delorean/FixedConfig.hpp>

#include <sstream>

NullInterval::NullInterval(timestamp_t start, timestamp_t end, attribute_t attribute)
: AbstractInterval(start, end, attribute, SIT_NULL)
{
}

std::string NullInterval::getStringValue(void) const
{
	return "(null)";
}

void NullInterval::serializeValues(uint8_t* var_addr, uint8_t* u32_addr) const {
	/* We don't need to write anything */
}

unsigned int NullInterval::unserializeValues(uint8_t* var_addr, uint8_t* u32_addr) {
	/* We don't need to read anything */
	
	return 0;
}

unsigned int NullInterval::getVariableValueSize(void) const {
	return 0;
}

AbstractInterval::SharedPtr NullInterval::clone(void) const{
	return AbstractInterval::SharedPtr(new NullInterval(*this));
}

