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
#ifndef _UINTINTERVAL_HPP
#define _UINTINTERVAL_HPP

#include <stdint.h>

#include "Interval.hpp"
#include "basic_types.h"

class UIntInterval : public Interval
{
public:
	UIntInterval(void) { }
	UIntInterval(timestamp_t start, timestamp_t end, attribute_t attribute, uint32_t value);
	~UIntInterval() { }
	std::string getStringValue(void) const;
	void serialize(void* var_addr, void* u32_addr) const;
	void unserialize(void* var_addr, void* u32_addr);
	unsigned int getVariableValueSize(void) const;
	Interval* clone(void) const;

private:
	uint32_t _value;
};

#endif // _UINTINTERVAL_HPP
