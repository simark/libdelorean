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
#include <cstring>
#include <cstdint>
#include <cstddef>
#include <memory>

#include <delorean/ex/UnknownNodeType.hpp>
#include <delorean/node/AbstractNode.hpp>
#include <delorean/node/LeafNode.hpp>
#include <delorean/node/BranchNode.hpp>
#include <delorean/node/NodeDeserializer.hpp>
#include <delorean/node/NodeTypes.hpp>
#include <delorean/BasicTypes.hpp>

#include "NodeStructs.hpp"

AbstractNode::UP NodeDeserializer::deserializeNode(const std::uint8_t* headPtr,
                                                   std::size_t size,
                                                   unsigned int maxChildren) const
{
    auto varEndPtr = headPtr + size;

    // read header
    NodeCommonHeader header;
    std::memcpy(&header, headPtr, sizeof(header));
    headPtr += NodeCommonHeader::SIZE;

    // read type
    auto type = header.getType();

    // create proper interval depending on type
    AbstractNode::UP node {};
    switch (type) {
    case static_cast<node_type_t>(NodeTypes::LEAF):
        node = std::move(LeafNode::UP {
            new LeafNode {
                size,
                header.seqNumber,
                header.parentSeqNumber,
                header.begin
            }
        });
        break;

    case static_cast<node_type_t>(NodeTypes::BRANCH):
        node = std::move(BranchNode::UP {
            new BranchNode {
                size,
                header.seqNumber,
                header.parentSeqNumber,
                header.begin,
                maxChildren
            }
        });
        break;

    default:
        throw UnknownNodeType {type};
    }

    // deserialize rest
    node->deserialize(headPtr, varEndPtr, header.intervalCount, *_deser);

    return node;
}
