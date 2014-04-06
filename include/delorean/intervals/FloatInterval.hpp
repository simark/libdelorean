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
#ifndef _FLOATINTERVAL_HPP
#define _FLOATINTERVAL_HPP

#include "../basic_types.h"
#include "../fixed_config.h"
#include "AbstractInterval.hpp"

class FloatInterval : public AbstractInterval
{
public:
	typedef std::tr1::shared_ptr<FloatInterval> SharedPtr;
	
	FloatInterval(void) : AbstractInterval(SIT_FLOAT32) { }
	FloatInterval(timestamp_t start, timestamp_t end, attribute_t attribute, float value);
	std::string getStringValue(void) const;
	unsigned int getVariableValueSize(void) const;
	AbstractInterval::SharedPtr clone(void) const;
	
	void setValue(float value) {_value = value;}
	float getValue() const {return _value;}
	
	static const interval_type_t type = SIT_FLOAT32;

protected:
	void serializeValues(uint8_t* var_addr, uint8_t* u32_addr) const;
	unsigned int unserializeValues(uint8_t* var_addr, uint8_t* u32_addr);

private:
	float _value;
};

#endif // _FLOATINTERVAL_HPP