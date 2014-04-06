/**
 * Copyright (c) 2012 Philippe Proulx <philippe.proulx@polymtl.ca>
 *
 * This file is part of libdelorean.
 *
 * libdelorean is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libdelorean is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libdelorean.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <cstdlib>
#include <sstream>

#include <delorean/IntervalCreator.hpp>
#include <delorean/intervals/IIntervalFactory.hpp>
#include <delorean/intervals/IntIntervalFactory.hpp>
#include <delorean/intervals/UIntIntervalFactory.hpp>
#include <delorean/intervals/StringIntervalFactory.hpp>
#include <delorean/intervals/FloatIntervalFactory.hpp>
#include <delorean/intervals/NullIntervalFactory.hpp>
#include <delorean/fixed_config.h>
#include <delorean/basic_types.h>
#include <delorean/ex/UnknownIntervalTypeEx.hpp>
#include <delorean/ex/ExistingIntervalTypeEx.hpp>

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
	this->registerIntervalType(SIT_NULL, new NullIntervalFactory());
}

IntervalCreator::~IntervalCreator(void) {
	this->unregisterAll();
}

void IntervalCreator::unregisterIntervalType(interval_type_t type) {
	if (this->_factories[type] != NULL) {
		delete this->_factories[type];
		this->_factories[type] = NULL;
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

AbstractInterval::SharedPtr IntervalCreator::createIntervalFromType(interval_type_t type) const throw(UnknownIntervalTypeEx) {
	if (this->_factories[type] == NULL) {
		throw UnknownIntervalTypeEx(type);
	}
	
	return this->_factories[type]->create();
}
