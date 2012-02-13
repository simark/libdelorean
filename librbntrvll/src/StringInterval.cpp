#include <sstream>
#include <cstring>

#include "StringInterval.hpp"
#include "basic_types.h"

using namespace std;

StringInterval::StringInterval(timestamp_t start, timestamp_t end, attribute_t attribute, string value)
: Interval(start, end, attribute), _value(value)
{
}

StringInterval::StringInterval(timestamp_t start, timestamp_t end, attribute_t attribute, const char* value)
: Interval(start, end, attribute), _value(value)
{
}

std::string StringInterval::getStringValue(void) const
{
	return _value;
}

void StringInterval::serialize(void* var_addr, void* u32_addr) const {
	// copy string length (enables faster reads than ASCIIZ)
	uint32_t sz = _value.size();
	memcpy(var_addr, &sz, sizeof(sz));
	
	// copy string
	memcpy((unsigned char*) var_addr + sizeof(sz), _value.c_str(), _value.size());
}

void StringInterval::unserialize(void* var_addr, void* u32_addr) {
	// read string length
	uint32_t sz;
	memcpy(&sz, var_addr, sizeof(sz));
	
	// read string
	_value.assign((char*) var_addr + sizeof(sz), sz);
}

unsigned int StringInterval::getVariableValueSize(void) const {
	// length of string + 32-bit header
	return sizeof(uint32_t) + _value.size();
}
