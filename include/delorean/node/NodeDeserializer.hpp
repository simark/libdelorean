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
#ifndef _NODEDESERIALIZER_HPP
#define _NODEDESERIALIZER_HPP

#include <vector>
#include <cstdint>

#include <delorean/node/AbstractNode.hpp>
#include <delorean/interval/IntervalDeserializer.hpp>
#include <delorean/BasicTypes.hpp>

class NodeDeserializer
{
public:
    NodeDeserializer(std::unique_ptr<IntervalDeserializer> deser) :
        _deser {std::move(deser)}
    {
    }

    ~NodeDeserializer()
    {
    }

    AbstractNode::UP deserializeNode(const std::uint8_t* headPtr,
                                     std::size_t size,
                                     unsigned int maxChildren) const;

private:
    std::unique_ptr<IntervalDeserializer> _deser;
};

#endif // _NODEDESERIALIZER_HPP
