#include <sstream>
#include <iomanip>
#include <iostream>

#include "Interval.hpp"
#include "IntInterval.hpp"

using namespace std;

Interval::Interval(uint64_t start, uint64_t end, uint32_t attribute)
: _start(start), _end(end), _attribute(attribute)
{
}

std::string Interval::toString(void) const
{	
	std::ostringstream oss;
	oss << "[" << this->_start << " to " << this->_end << "] " <<
		"[key = " << std::setw(5) << this->_attribute << "] " <<
		"[value = " << this->getStringValue() << "]";
	return oss.str();
}

bool Interval::intersects(uint64_t ts) const {
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
