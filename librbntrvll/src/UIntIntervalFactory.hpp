#ifndef _UINTINTERVALFACTORY_HPP
#define _UINTINTERVALFACTORY_HPP

#include "Interval.hpp"
#include "IIntervalFactory.hpp"

class UIntIntervalFactory : public IIntervalFactory
{
public:
	IntervalSharedPtr create(void) const;
	~UIntIntervalFactory(void) { }
};

#endif // _UINTINTERVALFACTORY_HPP
