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
#ifndef _INTERVAL_HPP
#define _INTERVAL_HPP

#include <string>
#include <tr1/memory>
#include <iostream>

#include "basic_types.h"
#include "IPrintable.hpp"

class Interval;

typedef std::tr1::shared_ptr<Interval> IntervalSharedPtr;

class Interval : public IPrintable
{
public:
	Interval(interval_type_t type) : _type(type) { }
	Interval(timestamp_t start, timestamp_t end, attribute_t attribute, interval_type_t type);
	virtual ~Interval() { }
	virtual std::string getStringValue(void) const = 0;
	void serialize(uint8_t* var_ptr, uint8_t* head_ptr);
	virtual void unserialize(void* var_addr, void* u32_addr) = 0;
	virtual unsigned int getVariableValueSize(void) const = 0;
	virtual Interval* clone(void) const = 0;
	unsigned int getTotalSize(void) const {
		return Interval::HEADER_SIZE + this->getVariableValueSize();
	}
	static unsigned int getHeaderSize(void) {
		return Interval::HEADER_SIZE;
	}
	std::string toString(void) const;
	bool intersects(timestamp_t ts) const;
	Interval* setInterval(timestamp_t start, timestamp_t end) {
		this->_start = start;
		this->_end = end;
		
		return this;
	}
	Interval* setStart(timestamp_t start) {
		this->_start = start;
		
		return this;
	}
	timestamp_t getStart(void) const {
		return this->_start;
	}
	Interval* setEnd(timestamp_t end) {
		this->_end = end;
		
		return this;
	}
	timestamp_t getEnd(void) const {
		return this->_end;
	}
	Interval* setAttribute(attribute_t attr) {
		this->_attribute = attr;
		
		return this;
	}
	timestamp_t getAttribute(void) const {
		return this->_attribute;
	}
	uint8_t getType(void) const {
		return this->_type;
	}
	
	bool operator==(const Interval& other);
	bool operator<(const Interval& other);
	bool operator<=(const Interval& other);
	bool operator!=(const Interval& other);
	bool operator>(const Interval& other);
	bool operator>=(const Interval& other);
	
protected:
	friend std::ostream& operator<<(std::ostream& out, const Interval& intr);
	virtual void serializeValues(void* var_addr, void* u32_addr) const = 0;
	static const unsigned int HEADER_SIZE;

private:
	// beggining and end of this interval
	timestamp_t _start;
	timestamp_t _end;
	
	// this is a unique integer ID for this interval
	attribute_t _attribute;
	
	// type ID
	interval_type_t _type;
};

#endif // _INTERVAL_HPP
