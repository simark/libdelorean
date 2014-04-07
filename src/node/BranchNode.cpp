/**
 * Copyright (c) 2014 Philippe Proulx <eepp.ca>
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
#include <algorithm>
#include <ostream>
#include <cstring>

#include <delorean/node/AbstractNode.hpp>
#include <delorean/node/BranchNode.hpp>
#include <delorean/node/NodeTypes.hpp>
#include <delorean/ex/BranchNodeFull.hpp>
#include <delorean/BasicTypes.hpp>

BranchNode::BranchNode(std::size_t size, node_seq_t seqNumber,
                       node_seq_t parentSeqNumber, timestamp_t begin,
                       unsigned int maxChildren) :
    AbstractNode {
        size,
        seqNumber,
        parentSeqNumber,
        begin,
        static_cast<node_type_t>(NodeTypes::BRANCH)
    },
    _maxChildren {maxChildren}
{
    // this should avoid unnecessary reallocations as children are added
    _children.reserve(maxChildren);
}

BranchNode::~BranchNode()
{
}

void BranchNode::addChild(const AbstractNode& childNode)
{
    // node full?
    if (this->isFull()) {
        throw BranchNodeFull();
    }

    _children.push_back({
        .begin = childNode.getBegin(),
        .seq = childNode.getSeqNumber()
    });
}

node_seq_t BranchNode::getChildSeqAtTs(timestamp_t ts) const
{
	auto potentialNextSeqNumber = this->getSeqNumber();

    for (const auto& child : _children) {
        if (ts >= child.begin) {
            potentialNextSeqNumber = child.seq;
        } else {
            break;
        }
    }

	return potentialNextSeqNumber;
}

std::size_t BranchNode::getSpecificHeaderSize() const
{
    return sizeof(uint32_t) +
        this->getChildrenCount() * ChildNodePointer::SIZE;
}

void BranchNode::deserializeSpecificHeader(const std::uint8_t* specificHeadPtr)
{
    // read number of children
    uint32_t childrenCount;
    std::memcpy(&childrenCount, specificHeadPtr, sizeof(childrenCount));
    specificHeadPtr += sizeof(childrenCount);

    // read this number of children
    _children.clear();
    for (unsigned int i = 0; i < childrenCount; ++i) {
        ChildNodePointer child;
        std::memcpy(&child, specificHeadPtr, ChildNodePointer::SIZE);
        _children.push_back(child);
        specificHeadPtr += ChildNodePointer::SIZE;
    }
}

void BranchNode::serializeSpecificHeader(std::uint8_t* specificHeadPtr) const
{
    // write number of children
    uint32_t childrenCount = _children.size();
    std::memcpy(specificHeadPtr, &childrenCount, sizeof(childrenCount));
    specificHeadPtr += sizeof(childrenCount);

    // write children
    for (const auto& child : _children) {
        std::memcpy(specificHeadPtr, &child, ChildNodePointer::SIZE);
        specificHeadPtr += ChildNodePointer::SIZE;
    }
}
