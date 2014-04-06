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
#ifndef _INTERVALCREATOR_HPP
#define _INTERVALCREATOR_HPP

#include "intervals/AbstractInterval.hpp"
#include "intervals/IIntervalFactory.hpp"
#include "ex/UnknownIntervalTypeEx.hpp"
#include "ex/ExistingIntervalTypeEx.hpp"
#include "BasicTypes.hpp"

class IntervalCreator
{
public:
    IntervalCreator(void);
    ~IntervalCreator(void);
    void registerIntervalType(interval_type_t type, IIntervalFactory* factory) throw(ExistingIntervalTypeEx);
    void unregisterIntervalType(interval_type_t type);
    void unregisterAll(void);
    AbstractInterval::SharedPtr createIntervalFromType(interval_type_t type) const throw(UnknownIntervalTypeEx);

private:
    IIntervalFactory* _factories [256];
};

#endif // _INTERVALCREATOR_HPP
