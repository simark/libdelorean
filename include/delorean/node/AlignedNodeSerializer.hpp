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
#ifndef _ALIGNEDNODESERIALIZER_HPP
#define _ALIGNEDNODESERIALIZER_HPP

#include <vector>
#include <cstdint>

#include <delorean/node/Node.hpp>
#include <delorean/interval/AbstractInterval.hpp>
#include <delorean/node/ChildNodePointer.hpp>
#include <delorean/BasicTypes.hpp>

class AlignedNodeSerializer
{
public:
    virtual ~AlignedNodeSerializer();

    void serializeNode(const Node& node,
                               std::uint8_t* headPtr) const;
    std::size_t getHeaderSize(const Node& node);
    std::size_t getChildNodePointerSize(const ChildNodePointer& cnp);
    std::size_t getIntervalSize(const AbstractInterval& interval);

protected:
    void serializeInterval
};

#endif // _ALIGNEDNODESERIALIZER_HPP
