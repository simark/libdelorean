#include "IntInterval.hpp"

#include <sstream>

IntInterval::IntInterval(uint64_t start, uint64_t end, uint32_t attribute, int32_t value)
:Interval(start, end, attribute), _value(value)
{
}

std::string IntInterval::getStringValue(void) const
{
	std::ostringstream oss;
	oss << _value;
	return oss.str();
}

void IntInterval::serialize(void* var_addr, void* u32_addr) const {
	*((int32_t*) u32_addr) = this->_value;
}

void IntInterval::unserialize(void* var_addr, void* u32_addr) {
	this->_value = *((int32_t*) u32_addr);
}

unsigned int IntInterval::getVariableValueSize(void) const {
	return 0;
}
