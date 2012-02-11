#include "Interval.hpp"
#include "UIntInterval.hpp"
#include "UIntIntervalFactory.hpp"

IntervalSharedPtr UIntIntervalFactory::create(void) const {
	IntervalSharedPtr interval(new UIntInterval());
	
	return interval;
}
