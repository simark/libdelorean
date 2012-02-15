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
	Interval(void) { }
	Interval(timestamp_t start, timestamp_t end, attribute_t attribute);
	virtual ~Interval() { }
	virtual std::string getStringValue(void) const = 0;
	virtual void serialize(void* var_addr, void* u32_addr) const = 0;
	virtual void unserialize(void* var_addr, void* u32_addr) = 0;
	virtual unsigned int getVariableValueSize(void) const = 0;
	virtual Interval* clone(void) const = 0;
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
	
	virtual int getVariableEntrySize() const{
		return _variableEntrySize;
	}
	
	/**
	 * Returns the size of the Interval without the variable-size data
	 * This is common for all Intervals 
	 */
	static int getStaticEntrySize(){
		return    16	/* 2 x Timevalue/long (interval start + end) */
			 + 4	/* int (key) */
			 + 1	/* byte (type) */
			 + 4;	/* int (valueOffset) */
		      /* = 25 */
	}
	
	int getIntervalSize() const {
		return getVariableEntrySize() + getStaticEntrySize();
	}	
	
	bool operator==(const Interval& other);
	bool operator<(const Interval& other);
	bool operator<=(const Interval& other);
	bool operator!=(const Interval& other);
	bool operator>(const Interval& other);
	bool operator>=(const Interval& other);
	
protected:
	//FIXME should this be declared here? It is derived-class specific
	int _variableEntrySize;
	friend std::ostream& operator<<(std::ostream& out, const Interval& intr);

private:
	// beggining and end of this interval
	timestamp_t _start;
	timestamp_t _end;
	
	// this is a unique integer ID for this interval
	attribute_t _attribute;
};

#endif // _INTERVAL_HPP
