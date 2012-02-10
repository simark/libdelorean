#ifndef _INTINTERVAL_HPP
#define _INTINTERVAL_HPP

#include "Interval.hpp"

class IntInterval : public Interval
{
public:
	IntInterval(void) { }
	IntInterval(uint64_t start, uint64_t end, int attribute, int value);
	std::string getStringValue(void) const;
	void serialize(void* var_addr, void* u32_addr) const;
	void unserialize(void* var_addr, void* u32_addr);
	unsigned int getVariableValueSize(void) const;

private:
	int32_t _value;
};

#endif // _INTINTERVAL_HPP
