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
	std::string toString(void) const;
	bool intersects(uint64_t ts) const;
	Interval* setInterval(timestamp_t start, timestamp_t end) {
		this->_start = start;
		this->_end = end;
		
		return this;
	}
	Interval* setStart(timestamp_t start) {
		this->_start = start;
		
		return this;
	}
	uint64_t getStart(void) const {
		return this->_start;
	}
	Interval* setEnd(timestamp_t end) {
		this->_end = end;
		
		return this;
	}
	uint64_t getEnd(void) const {
		return this->_end;
	}
	Interval* setAttribute(attribute_t attr) {
		this->_attribute = attr;
		
		return this;
	}
	uint64_t getAttribute(void) const {
		return this->_attribute;
	}
	bool operator==(const Interval& other);
	bool operator<(const Interval& other);
	bool operator<=(const Interval& other);
	bool operator!=(const Interval& other);
	bool operator>(const Interval& other);
	bool operator>=(const Interval& other);
	friend std::ostream& operator<<(std::ostream& out, const Interval& intr);

private:
	// beggining and end of this interval
	timestamp_t _start;
	timestamp_t _end;
	
	// this is a unique integer ID for this interval
	attribute_t _attribute;
};

#endif // _INTERVAL_HPP
