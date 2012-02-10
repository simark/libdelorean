#ifndef _UINTINTERVAL_HPP
#define _UINTINTERVAL_HPP

#include "Interval.hpp"

class UIntInterval : public Interval
{
public:
	UIntInterval(void) { }
	UIntInterval(uint64_t start, uint64_t end, int attribute, int value);
	~UIntInterval();
	std::string getStringValue(void) const;
	void serialize(void* var_addr, void* u32_addr) const;
	void unserialize(void* var_addr, void* u32_addr);
	unsigned int getVariableValueSize(void) const;

private:
	uint32_t _value;
};

#endif // _UINTINTERVAL_HPP
