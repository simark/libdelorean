#ifndef _INTERVAL_HPP
#define _INTERVAL_HPP

#include <stdint.h>
#include <string>
#include <tr1/memory>
#include <iostream>

#include "IPrintable.hpp"

class Interval : public IPrintable
{
public:
	Interval(uint64_t start, uint64_t end, int attribute);
	virtual ~Interval();
	virtual std::string getStringValue(void) const = 0;
	static std::tr1::shared_ptr<Interval> readFrom(/*Buffer*/);
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
	
	friend std::ostream& operator<<(const std::ostream& out, const Interval& intr);

private:
	uint64_t _start;
	uint64_t _end;
	
	// this integer identifies the attribute (quark)
	int _attribute;
	//No value for this attribute is defined here
	//It is derived-class specific
};

#endif // _IINTERVAL_HPP
