#ifndef _FLOATINTERVALFACTORY_HPP
#define _FLOATINTERVALFACTORY_HPP

#include "Interval.hpp"
#include "IIntervalFactory.hpp"

class FloatIntervalFactory : public IIntervalFactory
{
public:
	IntervalSharedPtr create(void) const;
	~FloatIntervalFactory(void) { }
};

#endif // _FLOATINTERVALFACTORY_HPP
