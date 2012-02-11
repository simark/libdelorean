#include <cstdlib>

#include "IntervalCreator.hpp"
#include "IIntervalFactory.hpp"
#include "IntIntervalFactory.hpp"
#include "IntInterval.hpp"
#include "simple_interval_types.h"
#include "ex/UnknownIntervalTypeEx.hpp"
#include "ex/ExistingIntervalTypeEx.hpp"

IntervalCreator::IntervalCreator(void) {
	// reset all factories
	for (unsigned int i = 0; i < 256; ++i) {
		this->_factories[i] = NULL;
	}
	
	// register simple ones (we know them)
	this->registerIntervalType(IST_INT32, new IntIntervalFactory());
}

void IntervalCreator::unregisterIntervalType(uint8_t type) {
	if (this->_factories[type] != NULL) {
		delete this->_factories[type];
	}
}

void IntervalCreator::registerIntervalType(uint8_t type, IIntervalFactory* factory) throw(ExistingIntervalTypeEx) {
	if (this->_factories[type] == NULL) {
		throw new ExistingIntervalTypeEx();
	} else {
		this->_factories[type] = factory;
	}
}

void IntervalCreator::unregisterAll(void) {
	for (unsigned int i = 0; i < 256; ++i) {
		this->unregisterIntervalType(i);
	}
}

IntervalSharedPtr IntervalCreator::createIntervalFromType(uint8_t type) throw(UnknownIntervalTypeEx) {
	if (this->_factories[type] == NULL) {
		throw new UnknownIntervalTypeEx();
	}
	
	return this->_factories[type]->create();
}
