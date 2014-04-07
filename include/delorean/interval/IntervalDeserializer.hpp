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
#ifndef _INTERVALDESERIALIZER_HPP
#define _INTERVALDESERIALIZER_HPP

#include <vector>
#include <cstdint>

#include <delorean/interval/AbstractInterval.hpp>
#include <delorean/interval/IIntervalFactory.hpp>
#include <delorean/interval/StandardIntervalTypes.hpp>
#include <delorean/BasicTypes.hpp>

class IntervalDeserializer
{
public:
    IntervalDeserializer();
    ~IntervalDeserializer();

    void registerIntervalType(interval_type_t type,
                              IIntervalFactory::UP factory);
    void unregisterIntervalType(interval_type_t type);
    void unregisterAll();
    AbstractInterval::UP deserializeInterval(const std::uint8_t* headPtr,
                                             const std::uint8_t* varEndPtr) const;

private:
    void registerIntervalType(StandardIntervalTypes type,
                              std::unique_ptr<IIntervalFactory> factory);

private:
    std::vector<std::unique_ptr<IIntervalFactory>> _factories;
};

#endif // _INTERVALDESERIALIZER_HPP
