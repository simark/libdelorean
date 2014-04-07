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
#ifndef _LEAFNODE_HPP
#define _LEAFNODE_HPP

#include <memory>
#include <cstdint>
#include <vector>

#include <delorean/node/AbstractNode.hpp>
#include <delorean/BasicTypes.hpp>

class LeafNode :
    public AbstractNode
{
public:
    typedef std::shared_ptr<LeafNode> SP;
    typedef std::unique_ptr<LeafNode> UP;
    typedef std::shared_ptr<const LeafNode> CSP;
    typedef std::unique_ptr<const LeafNode> CUP;

public:
    LeafNode(std::size_t size, node_seq_t seqNumber,
             node_seq_t parentSeqNumber, timestamp_t begin);
    virtual ~LeafNode();

protected:
    std::size_t getSpecificHeaderSize() const;
    void deserializeSpecificHeader(const std::uint8_t* specificHeadPtr);
    void serializeSpecificHeader(std::uint8_t* specificHeadPtr) const;
};

#endif // _LEAFNODE_HPP
