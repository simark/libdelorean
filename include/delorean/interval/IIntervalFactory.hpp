/**
 * Copyright (c) 2014 Philippe Proulx <eepp.ca>
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
#ifndef _IINTERVALFACTORY_HPP
#define _IINTERVALFACTORY_HPP

#include <memory>
#include <cstdint>

#include <delorean/interval/AbstractInterval.hpp>
#include <delorean/BasicTypes.hpp>

class IIntervalFactory
{
public:
    typedef std::shared_ptr<IIntervalFactory> SP;
    typedef std::unique_ptr<IIntervalFactory> UP;

public:
    virtual ~IIntervalFactory()
    {
    }

    virtual AbstractInterval::UP create(timestamp_t begin, timestamp_t end,
                                        interval_id_t id) const = 0;
};

#endif // _IINTERVALFACTORY_HPP