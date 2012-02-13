#include <sstream>
#include <iomanip>
#include <iostream>

#include "basic_types.h"
#include "Interval.hpp"
#include "IntInterval.hpp"

using namespace std;

Interval::Interval(timestamp_t start, timestamp_t end, attribute_t attribute)
: _start(start), _end(end), _attribute(attribute)
{
}

std::string Interval::toString(void) const
{	
	ostringstream oss;
	oss << "[" << this->_start << " to " << this->_end << "] " <<
		"[key = " << std::setw(5) << this->_attribute << "] " <<
		"[value = " << this->getStringValue() << "]";
	return oss.str();
}

bool Interval::intersects(timestamp_t ts) const {
	return this->_start <= ts && this->_end >= ts;
}

ostream& operator<<(ostream& out, Interval const& intr)
{
	out << intr.toString();
	
	return out;
}

bool Interval::operator==(const Interval& other)
{
	return this->_end == other._end;
}

bool Interval::operator<(const Interval& other)
{
	return this->_end < other._end;
}

bool Interval::operator<=(const Interval& other)
{
	return this->_end <= other._end;
}

bool Interval::operator!=(const Interval& other)
{
	return this->_end != other._end;
}

bool Interval::operator>(const Interval& other)
{
	return this->_end > other._end;
}

bool Interval::operator>=(const Interval& other)
{
	return this->_end >= other._end;
}
