#ifndef _INTINTERVALFACTORY_HPP
#define _INTINTERVALFACTORY_HPP

#include "Interval.hpp"
#include "IIntervalFactory.hpp"

class IntIntervalFactory : public IIntervalFactory
{
public:
	IntervalSharedPtr create(void) const;
	~IntIntervalFactory(void) { }
};

#endif // _INTINTERVALFACTORY_HPP
