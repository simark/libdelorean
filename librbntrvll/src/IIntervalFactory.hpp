#ifndef _IINTERVALFACTORY_HPP
#define _IINTERVALFACTORY_HPP

#include <tr1/memory>

#include "Interval.hpp"

class IIntervalFactory
{
public:
	virtual IntervalSharedPtr create(void) const = 0;
	virtual ~IIntervalFactory(void) { }
};

#endif // _IINTERVALFACTORY_HPP
