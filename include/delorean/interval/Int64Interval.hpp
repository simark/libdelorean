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
#ifndef _INT64INTERVAL_HPP
#define _INT64INTERVAL_HPP

#include <cstdint>

#include <delorean/interval/Simple64BitValueInterval.hpp>
#include <delorean/interval/StandardIntervalType.hpp>
#include <delorean/BasicTypes.hpp>

namespace delo
{

/**
 * Interval containing a single 32-bit signed integer value.
 *
 * @author Philippe Proulx
 */
class Int64Interval :
    public Simple64BitValueInterval<std::int64_t, StandardIntervalType::INT64>
{
public:
    typedef std::shared_ptr<Int64Interval> SP;
    typedef std::unique_ptr<Int64Interval> UP;

public:
    using Simple64BitValueInterval::Simple64BitValueInterval;
};

}

#endif // _INT64INTERVAL_HPP
