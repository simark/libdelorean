#include "Interval.hpp"
#include "FloatInterval.hpp"
#include "FloatIntervalFactory.hpp"

IntervalSharedPtr FloatIntervalFactory::create(void) const {
	IntervalSharedPtr interval(new FloatInterval());
	
	return interval;
}
