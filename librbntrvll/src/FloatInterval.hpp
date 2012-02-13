#ifndef _FLOATINTERVAL_HPP
#define _FLOATINTERVAL_HPP

#include "basic_types.h"

#include "Interval.hpp"

class FloatInterval : public Interval
{
public:
	FloatInterval(void) { }
	FloatInterval(timestamp_t start, timestamp_t end, attribute_t attribute, float value);
	std::string getStringValue(void) const;
	void serialize(void* var_addr, void* u32_addr) const;
	void unserialize(void* var_addr, void* u32_addr);
	unsigned int getVariableValueSize(void) const;

private:
	float _value;
};

#endif // _FLOATINTERVAL_HPP
