#ifndef _STRINGINTERVALFACTORY_HPP
#define _STRINGINTERVALFACTORY_HPP

#include "Interval.hpp"
#include "IIntervalFactory.hpp"

class StringIntervalFactory : public IIntervalFactory
{
public:
	IntervalSharedPtr create(void) const;
	~StringIntervalFactory(void) { }
};

#endif // _STRINGINTERVALFACTORY_HPP
