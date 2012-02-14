/**
 * Copyright (c) 2012 Philippe Proulx <philippe.proulx@polymtl.ca>
 * Copyright (c) 2012 François Rajotte <francois.rajotte@polymtl.ca>
 *
 * This file is part of librbntrvll.
 *
 * librbntrvll is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * librbntrvll is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with librbntrvll.  If not, see <http://www.gnu.org/licenses/>.
 */
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
