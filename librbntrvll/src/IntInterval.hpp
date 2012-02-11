#ifndef _INTINTERVAL_HPP
#define _INTINTERVAL_HPP

#include <stdint.h>

#include "Interval.hpp"

class IntInterval : public Interval
{
public:
	IntInterval(void) { }
	IntInterval(uint64_t start, uint64_t end, uint32_t attribute, int32_t value);
	std::string getStringValue(void) const;
	void serialize(void* var_addr, void* u32_addr) const;
	void unserialize(void* var_addr, void* u32_addr);
	unsigned int getVariableValueSize(void) const;

private:
	int32_t _value;
};

#endif // _INTINTERVAL_HPP
