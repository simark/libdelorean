#ifndef _UINTINTERVAL_HPP
#define _UINTINTERVAL_HPP

#include <stdint.h>

#include "Interval.hpp"
#include "basic_types.h"

class UIntInterval : public Interval
{
public:
	UIntInterval(void) { }
	UIntInterval(timestamp_t start, timestamp_t end, attribute_t attribute, uint32_t value);
	~UIntInterval() { }
	std::string getStringValue(void) const;
	void serialize(void* var_addr, void* u32_addr) const;
	void unserialize(void* var_addr, void* u32_addr);
	unsigned int getVariableValueSize(void) const;

private:
	uint32_t _value;
};

#endif // _UINTINTERVAL_HPP
