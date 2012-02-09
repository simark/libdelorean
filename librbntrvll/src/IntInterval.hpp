#ifndef _INTINTERVAL_HPP
#define _INTINTERVAL_HPP

#include "Interval.hpp"

class IntInterval : public Interval
{
public:
	IntInterval(uint64_t start, uint64_t end, int attribute, int value);
	IntInterval(uint64_t start, uint64_t end, int attribute/*, Buffer*/);
	virtual ~IntInterval();
	virtual std::string getStringValue(void) const;
	
private:
	int _value;
};

#endif // _INTINTERVAL_HPP
