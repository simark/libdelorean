/**
 * Copyright (c) 2014 Philippe Proulx <eepp.ca>
 * Copyright (c) 2012 François Rajotte <francois.rajotte@polymtl.ca>
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
#ifndef _INTINTERVAL_HPP
#define _INTINTERVAL_HPP

#include <cstdint>

#include <delorean/interval/SimpleValueInterval.hpp>
#include <delorean/interval/StandardIntervalType.hpp>
#include <delorean/BasicTypes.hpp>

/**
 * Interval containing a single 32-bit signed integer value.
 *
 * @author Philippe Proulx
 */
class IntInterval :
    public SimpleValueInterval<std::int32_t, StandardIntervalType::INT32>
{
public:
    typedef std::shared_ptr<IntInterval> SP;
    typedef std::unique_ptr<IntInterval> UP;

public:
    using SimpleValueInterval::SimpleValueInterval;
};

#endif // _INTINTERVAL_HPP
