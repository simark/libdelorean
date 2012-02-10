#include "Interval.hpp"
#include "IntInterval.hpp"
#include "IntIntervalFactory.hpp"

IntervalSharedPtr IntIntervalFactory::create(void) const {
	IntervalSharedPtr interval(new IntInterval());
	
	return interval;
}
