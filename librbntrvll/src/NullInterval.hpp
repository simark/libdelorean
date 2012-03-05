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
#ifndef _NULLINTERVAL_HPP
#define _NULLINTERVAL_HPP

#include <stdint.h>

#include "fixed_config.h"
#include "Interval.hpp"

class NullInterval : public Interval
{
public:
	NullInterval(void) : Interval(SIT_NULL) { }
	NullInterval(timestamp_t start, timestamp_t end, attribute_t attribute);

	std::string getStringValue(void) const;
	unsigned int getVariableValueSize(void) const;
	
protected:
	void serializeValues(uint8_t* var_addr, uint8_t* u32_addr) const;
	unsigned int unserializeValues(uint8_t* var_addr, uint8_t* u32_addr);
};

#endif // _NULLINTERVAL_HPP
