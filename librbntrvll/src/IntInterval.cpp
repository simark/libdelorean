#include "IntInterval.hpp"

#include <sstream>

IntInterval::IntInterval(uint64_t start, uint64_t end, int attribute, int value)
:Interval(start, end, attribute), _value(value)
{
}

IntInterval::IntInterval(uint64_t start, uint64_t end, int attribute/*, Buffer*/)
:Interval(start, end, attribute)
{
}

IntInterval::~IntInterval()
{
}

std::string IntInterval::getStringValue(void) const
{
	std::ostringstream oss;
	oss << _value;
	return oss.str();
}

