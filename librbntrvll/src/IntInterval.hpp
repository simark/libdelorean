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
#ifndef _INTINTERVAL_HPP
#define _INTINTERVAL_HPP

#include <stdint.h>

#include "fixed_config.h"
#include "Interval.hpp"

class IntInterval : public Interval
{
public:
	typedef std::tr1::shared_ptr<IntInterval> SharedPtr;
	
	IntInterval(void) : Interval(SIT_INT32) { }
	IntInterval(timestamp_t start, timestamp_t end, attribute_t attribute, int32_t value);
	std::string getStringValue(void) const;
	unsigned int getVariableValueSize(void) const;
	IntInterval* setValue(int32_t value) {
		this->_value = value;
		
		return this;
	}

protected:
	void serializeValues(uint8_t* var_addr, uint8_t* u32_addr) const;
	unsigned int unserializeValues(uint8_t* var_addr, uint8_t* u32_addr);

private:
	int32_t _value;
};

#endif // _INTINTERVAL_HPP
