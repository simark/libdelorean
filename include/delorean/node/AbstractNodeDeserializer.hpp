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
#ifndef _ABSTRACTNODEDESERIALIZER_HPP
#define _ABSTRACTNODEDESERIALIZER_HPP

#include <cstdint>

#include <delorean/node/Node.hpp>
#include <delorean/interval/IntervalDeserializer.hpp>
#include <delorean/BasicTypes.hpp>

class AbstractNodeDeserializer
{
public:
    virtual ~AbstractNodeDeserializer() = 0;

    Node::UP deserializeNode(const std::uint8_t* headPtr,
                                     std::size_t size,
                                     unsigned int maxChildren) const;
    void registerIntervalFactory(interval_type_t type,
                                 IIntervalFactory::UP factory);
    void unregisterIntervalFactory(interval_type_t type);
};

#endif // _ABSTRACTNODEDESERIALIZER_HPP
