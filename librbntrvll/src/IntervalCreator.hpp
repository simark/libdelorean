#ifndef _INTERVALCREATOR_HPP
#define _INTERVALCREATOR_HPP

#include "Interval.hpp"
#include "IIntervalFactory.hpp"
#include "ex/NonexistentFactoryEx.hpp"
#include "ex/FactoryOverwritingEx.hpp"

class IntervalCreator
{
public:
	IntervalCreator(void);
	void registerFactory(uint8_t type, IIntervalFactory* factory) throw(FactoryOverwritingEx);
	void unregisterFactory(uint8_t type);
	void unregisterAll(void);
	IntervalSharedPtr createFromType(uint8_t type) throw(NonexistentFactoryEx);

private:
	IIntervalFactory* _factories [256];
};

#endif // _INTERVALCREATOR_HPP
