#ifndef _STRINGINTERVAL_HPP
#define _STRINGINTERVAL_HPP

#include <string>

#include "Interval.hpp"

class StringInterval : public Interval
{
public:
	StringInterval(void) { }
	StringInterval(uint64_t start, uint64_t end, uint32_t attribute, std::string value);
	StringInterval(uint64_t start, uint64_t end, uint32_t attribute, const char* value);
	std::string getStringValue(void) const;
	void serialize(void* var_addr, void* u32_addr) const;
	void unserialize(void* var_addr, void* u32_addr);
	unsigned int getVariableValueSize(void) const;

private:
	std::string _value;
};

#endif // _STRINGINTERVAL_HPP
