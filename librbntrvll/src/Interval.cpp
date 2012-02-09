#include <sstream>
#include <iomanip>
#include <iostream>

#include "Interval.hpp"
#include "IntInterval.hpp"

using namespace std;

Interval::Interval(uint64_t start, uint64_t end, int attribute)
: _start(start), _end(end), _attribute(attribute)
{
}

Interval::~Interval()
{
}

std::tr1::shared_ptr<Interval> Interval::readFrom(/*Buffer*/)
{
	//Read start, end, attribute + other generic infos
	uint64_t start;
	uint64_t end;
	int attribute;
	int type;
	std::tr1::shared_ptr<Interval> interval;
	
	switch(type)
	{
		case 0: interval = std::tr1::shared_ptr<Interval>(new IntInterval(start, end, attribute/*Buffer*/));
			break;
		case 1:
			break;
		default:
			break;
	}
	return interval;
}

std::string Interval::toString(void) const
{
	/*
	StringBuffer buf = new StringBuffer(start + " to ");
        buf.append(end + "  ");
        buf.append(String.format("key = %4d, ", attribute));
        buf.append("value = " + sv.toString());
        return buf.toString();
        */
	
	std::ostringstream oss;
	oss << "[" << this->_start << " to " << this->_end << "] " <<
		"[key = " << std::setw(5) << this->_attribute << "] " <<
		"[value = " << this->getStringValue() << "]";
	return oss.str();
}

bool Interval::intersects(uint64_t ts) const {
	return this->_start <= ts && this->_end >= ts;
}

ostream& operator<<(ostream& out, Interval& intr)
{
	out << intr.toString();
	
	return out;
}
