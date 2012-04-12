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
#ifndef _CUSTOMINTERVAL_HPP
#define _CUSTOMINTERVAL_HPP

#include <string>

#include "intervals/AbstractInterval.hpp"
#include "basic_types.h"
#include "fixed_config.h"

typedef struct
{
	double x;
	double y;
} data;

class CustomInterval : public AbstractInterval
{
public:
	typedef std::tr1::shared_ptr<CustomInterval> SharedPtr;
	
	CustomInterval(void) : AbstractInterval(type) { }
	std::string getStringValue(void) const;
	unsigned int getVariableValueSize(void) const;
	virtual AbstractInterval::SharedPtr clone() const;
	void setValue(data value) {_value = value;}
	data getValue() const {return _value;}
	
	static const interval_type_t type = 5;

protected:
	void serializeValues(uint8_t* var_addr, uint8_t* u32_addr) const;
	unsigned int unserializeValues(uint8_t* var_addr, uint8_t* u32_addr);
	
private:
	data _value;
};

#endif // _CUSTOMINTERVAL_HPP
