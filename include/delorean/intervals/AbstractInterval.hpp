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
#ifndef _ABSTRACTINTERVAL_HPP
#define _ABSTRACTINTERVAL_HPP

#include <string>
#include <tr1/memory>
#include <iostream>

#include "../basic_types.h"
#include "../IPrintable.hpp"

class AbstractInterval : public IPrintable
{	
	friend std::ostream& operator<<(std::ostream& out, const AbstractInterval& intr);
	
public:
	typedef std::tr1::shared_ptr<AbstractInterval> SharedPtr;
	
	AbstractInterval(interval_type_t type) : _type(type) { }
	AbstractInterval(timestamp_t start, timestamp_t end, attribute_t attribute, interval_type_t type);
	virtual ~AbstractInterval() { }
	virtual std::string getStringValue(void) const = 0;
	void serialize(uint8_t* var_ptr, uint8_t* head_ptr);
	unsigned int unserialize(uint8_t* var_ptr, uint8_t* head_ptr);
	virtual unsigned int getVariableValueSize(void) const = 0;
	virtual AbstractInterval::SharedPtr clone() const = 0;
	unsigned int getTotalSize(void) const {
		return AbstractInterval::HEADER_SIZE + this->getVariableValueSize();
	}
	static unsigned int getHeaderSize(void) {
		return AbstractInterval::HEADER_SIZE;
	}
	std::string toString(void) const;
	bool intersects(timestamp_t ts) const;
	AbstractInterval* setInterval(timestamp_t start, timestamp_t end) {
		this->_start = start;
		this->_end = end;
		
		return this;
	}
	AbstractInterval* setStart(timestamp_t start) {
		this->_start = start;
		
		return this;
	}
	timestamp_t getStart(void) const {
		return this->_start;
	}
	AbstractInterval* setEnd(timestamp_t end) {
		this->_end = end;
		
		return this;
	}
	timestamp_t getEnd(void) const {
		return this->_end;
	}
	AbstractInterval* setAttribute(attribute_t attr) {
		this->_attribute = attr;
		
		return this;
	}
	timestamp_t getAttribute(void) const {
		return this->_attribute;
	}
	uint8_t getType(void) const {
		return this->_type;
	}
	
	bool operator==(const AbstractInterval& other);
	bool operator<(const AbstractInterval& other);
	bool operator<=(const AbstractInterval& other);
	bool operator!=(const AbstractInterval& other);
	bool operator>(const AbstractInterval& other);
	bool operator>=(const AbstractInterval& other);
	
protected:
	virtual void serializeValues(uint8_t* var_addr, uint8_t* u32_addr) const = 0;
	virtual unsigned int unserializeValues(uint8_t* var_addr, uint8_t* u32_addr) = 0;
	static const unsigned int HEADER_SIZE;

private:
	// beggining and end of this interval
	timestamp_t _start;
	timestamp_t _end;
	
	// integer ID for this interval
	attribute_t _attribute;
	
	// type identifier
	interval_type_t _type;
};

#endif // _ABSTRACTINTERVAL_HPP
