/* Copyright (c) 2014 Philippe Proulx <eepp.ca>
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
#ifndef _INT32INTERVAL_HPP
#define _INT32INTERVAL_HPP

#include <cstdint>

#include <delorean/interval/Simple32BitValueInterval.hpp>
#include <delorean/interval/StandardIntervalType.hpp>
#include <delorean/BasicTypes.hpp>

/**
 * Interval containing a single 32-bit signed integer value.
 *
 * @author Philippe Proulx
 */
class Int32Interval :
    public Simple32BitValueInterval<std::int32_t, StandardIntervalType::INT32>
{
public:
    typedef std::shared_ptr<Int32Interval> SP;
    typedef std::unique_ptr<Int32Interval> UP;

public:
    using Simple32BitValueInterval::Simple32BitValueInterval;
};

#endif // _INT32INTERVAL_HPP
