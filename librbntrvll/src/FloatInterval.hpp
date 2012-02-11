#ifndef _FLOATINTERVAL_HPP
#define _FLOATINTERVAL_HPP

#include <stdint.h>

#include "Interval.hpp"

class FloatInterval : public Interval
{
public:
	FloatInterval(void) { }
	FloatInterval(uint64_t start, uint64_t end, uint32_t attribute, float value);
	std::string getStringValue(void) const;
	void serialize(void* var_addr, void* u32_addr) const;
	void unserialize(void* var_addr, void* u32_addr);
	unsigned int getVariableValueSize(void) const;

private:
	float _value;
};

#endif // _FLOATINTERVAL_HPP
