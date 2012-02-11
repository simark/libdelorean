#include "Interval.hpp"
#include "StringInterval.hpp"
#include "StringIntervalFactory.hpp"

IntervalSharedPtr StringIntervalFactory::create(void) const {
	IntervalSharedPtr interval(new StringInterval());
	
	return interval;
}
