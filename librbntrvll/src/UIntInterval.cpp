#include "UIntInterval.hpp"

#include <sstream>

UIntInterval::UIntInterval(uint64_t start, uint64_t end, int attribute, int value)
:Interval(start, end, attribute), _value(value)
{
}

std::string UIntInterval::getStringValue(void) const
{
	std::ostringstream oss;
	oss << _value;
	return oss.str();
}

void UIntInterval::serialize(void* var_addr, void* u32_addr) const {
	*((uint32_t*) u32_addr) = this->_value;
}

void UIntInterval::unserialize(void* var_addr, void* u32_addr) {
	this->_value = *((uint32_t*) u32_addr);
}

unsigned int UIntInterval::getVariableValueSize(void) const {
	return 0;
}
