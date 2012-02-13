#include "FloatInterval.hpp"
#include "basic_types.h"

#include <sstream>

FloatInterval::FloatInterval(timestamp_t start, timestamp_t end, attribute_t attribute, float value)
:Interval(start, end, attribute), _value(value)
{
}

std::string FloatInterval::getStringValue(void) const
{
	std::ostringstream oss;
	oss << _value;
	return oss.str();
}

void FloatInterval::serialize(void* var_addr, void* u32_addr) const {
	*((float*) u32_addr) = this->_value;
}

void FloatInterval::unserialize(void* var_addr, void* u32_addr) {
	this->_value = *((float*) u32_addr);
}

unsigned int FloatInterval::getVariableValueSize(void) const {
	return 0;
}
