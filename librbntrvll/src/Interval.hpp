#ifndef _INTERVAL_HPP
#define _INTERVAL_HPP

#include <string>
#include <tr1/memory>
#include <iostream>
#include <stdint.h>

#include "IPrintable.hpp"

class Interval;

typedef std::tr1::shared_ptr<Interval> IntervalSharedPtr;

class Interval : public IPrintable
{
public:
	Interval(void) { }
	Interval(uint64_t start, uint64_t end, int attribute);
	virtual ~Interval();
	virtual std::string getStringValue(void) const = 0;
	virtual void serialize(void* var_addr, void* u32_addr) const = 0;
	virtual void unserialize(void* var_addr, void* u32_addr) = 0;
	virtual unsigned int getVariableValueSize(void) const = 0;
	std::string toString(void) const;
	bool intersects(uint64_t ts) const;
	uint64_t getStart(void) const {
		return _start;
	}
	uint64_t getEnd(void) const {
		return _end;
	}
	uint64_t getAttribute(void) const {
		return _attribute;
	}
	bool operator==(const Interval& other);
	bool operator<(const Interval& other);
	bool operator<=(const Interval& other);
	bool operator!=(const Interval& other);
	bool operator>(const Interval& other);
	bool operator>=(const Interval& other);
	friend std::ostream& operator<<(const std::ostream& out,
		const Interval& intr);
	static std::tr1::shared_ptr<Interval> create(void);

private:
	uint64_t _start;
	uint64_t _end;
	
	// this integer identifies the attribute (quark)
	int _attribute;
	//No value for this attribute is defined here
	//It is derived-class specific
};

#endif // _IINTERVAL_HPP
