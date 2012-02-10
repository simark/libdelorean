#include <cstdlib>

#include "IntervalCreator.hpp"
#include "IIntervalFactory.hpp"
#include "IntIntervalFactory.hpp"
#include "IntInterval.hpp"
#include "simple_interval_types.h"

IntervalCreator::IntervalCreator(void) {
	// reset all factories
	for (unsigned int i = 0; i < 256; ++i) {
		this->_factories[i] = NULL;
	}
	
	// register simple ones (we know them)
	this->registerFactory(IST_INT32, new IntIntervalFactory());
}

void IntervalCreator::unregisterFactory(uint8_t type) {
	if (this->_factories[type] != NULL) {
		delete this->_factories[type];
	}
}

void IntervalCreator::registerFactory(uint8_t type, IIntervalFactory* factory) throw(FactoryOverwritingEx) {
	if (this->_factories[type] == NULL) {
		throw new FactoryOverwritingEx;
	} else {
		this->_factories[type] = factory;
	}
}

void IntervalCreator::unregisterAll(void) {
	for (unsigned int i = 0; i < 256; ++i) {
		this->unregisterFactory(i);
	}
}

IntervalSharedPtr IntervalCreator::createFromType(uint8_t type) throw(NonexistentFactoryEx) {
	if (this->_factories[type] == NULL) {
		throw new NonexistentFactoryEx();
	}
	
	return this->_factories[type]->create();
}
