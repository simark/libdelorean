#ifndef _INTERVALCREATOR_HPP
#define _INTERVALCREATOR_HPP

#include "Interval.hpp"
#include "IIntervalFactory.hpp"
#include "ex/UnknownIntervalTypeEx.hpp"
#include "ex/ExistingIntervalTypeEx.hpp"

class IntervalCreator
{
public:
	IntervalCreator(void);
	void registerIntervalType(uint8_t type, IIntervalFactory* factory) throw(ExistingIntervalTypeEx);
	void unregisterIntervalType(uint8_t type);
	void unregisterAll(void);
	IntervalSharedPtr createIntervalFromType(uint8_t type) throw(UnknownIntervalTypeEx);

private:
	IIntervalFactory* _factories [256];
};

#endif // _INTERVALCREATOR_HPP
