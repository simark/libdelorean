#include <cstdlib>
#include <sstream>

#include "IntervalCreator.hpp"
#include "IIntervalFactory.hpp"
#include "IntIntervalFactory.hpp"
#include "UIntIntervalFactory.hpp"
#include "StringIntervalFactory.hpp"
#include "FloatIntervalFactory.hpp"
#include "fixed_config.h"
#include "basic_types.h"
#include "ex/UnknownIntervalTypeEx.hpp"
#include "ex/ExistingIntervalTypeEx.hpp"

IntervalCreator::IntervalCreator(void) {
	// reset all factories
	for (unsigned int i = 0; i < 256; ++i) {
		this->_factories[i] = NULL;
	}
	
	// register simple ones (we know them)
	this->registerIntervalType(SIT_INT32, new IntIntervalFactory());
	this->registerIntervalType(SIT_UINT32, new UIntIntervalFactory());
	this->registerIntervalType(SIT_STRING, new StringIntervalFactory());
	this->registerIntervalType(SIT_FLOAT32, new FloatIntervalFactory());
}

void IntervalCreator::unregisterIntervalType(interval_type_t type) {
	if (this->_factories[type] != NULL) {
		delete this->_factories[type];
	}
}

void IntervalCreator::registerIntervalType(interval_type_t type, IIntervalFactory* factory) throw(ExistingIntervalTypeEx) {
	if (this->_factories[type] != NULL) {
		std::ostringstream oss;
		oss << "interval type " << (unsigned int) type << " is already registered";
		throw ExistingIntervalTypeEx(oss.str());
	} else {
		this->_factories[type] = factory;
	}
}

void IntervalCreator::unregisterAll(void) {
	for (unsigned int i = 0; i < 256; ++i) {
		this->unregisterIntervalType(i);
	}
}

IntervalSharedPtr IntervalCreator::createIntervalFromType(interval_type_t type) throw(UnknownIntervalTypeEx) {
	if (this->_factories[type] == NULL) {
		throw UnknownIntervalTypeEx(type);
	}
	
	return this->_factories[type]->create();
}
