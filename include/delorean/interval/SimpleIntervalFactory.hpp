/* Copyright (c) 2014 Philippe Proulx <eepp.ca>
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
#ifndef _SIMPLEINTERVALFACTORY_HPP
#define _SIMPLEINTERVALFACTORY_HPP

#include <cstdint>

#include <delorean/interval/AbstractInterval.hpp>
#include <delorean/BasicTypes.hpp>

/**
 * Template class for creating simple interval factories.
 *
 * @author Philippe Proulx
 */
template<typename T>
class SimpleIntervalFactory :
    public IIntervalFactory
{
public:
    AbstractInterval::UP create(timestamp_t begin, timestamp_t end,
                                interval_key_t key) const;
};

template<typename T>
AbstractInterval::UP SimpleIntervalFactory<T>::create(timestamp_t begin,
                                                      timestamp_t end,
                                                      interval_key_t key) const
{
    auto intervalPtr = new T {begin, end, key};
    auto intervalUP = AbstractInterval::UP {intervalPtr};

    return std::move(intervalUP);
}

#endif // _SIMPLEINTERVALFACTORY_HPP
