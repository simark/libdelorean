#ifndef _INTERVALCREATOR_HPP
#define _INTERVALCREATOR_HPP

#include "Interval.hpp"
#include "IIntervalFactory.hpp"
#include "ex/UnknownIntervalTypeEx.hpp"
#include "ex/ExistingIntervalTypeEx.hpp"
#include "basic_types.h"

class IntervalCreator
{
public:
	IntervalCreator(void);
	void registerIntervalType(interval_type_t type, IIntervalFactory* factory) throw(ExistingIntervalTypeEx);
	void unregisterIntervalType(interval_type_t type);
	void unregisterAll(void);
	IntervalSharedPtr createIntervalFromType(interval_type_t type) throw(UnknownIntervalTypeEx);

private:
	IIntervalFactory* _factories [256];
};

#endif // _INTERVALCREATOR_HPP
