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
#include <memory>
#include <sstream>
#include <cstring>

#include <delorean/node/AlignedNodeSerDes.hpp>
#include <delorean/interval/SimpleIntervalFactory.hpp>
#include <delorean/interval/StandardIntervalType.hpp>
#include <delorean/node/Node.hpp>
#include <delorean/ex/UnknownIntervalType.hpp>
#include <delorean/ex/ExistingIntervalType.hpp>
#include <delorean/BasicTypes.hpp>

AlignedNodeSerDes::AlignedNodeSerDes()
{
}

AlignedNodeSerDes::~AlignedNodeSerDes()
{
}

void AlignedNodeSerDes::serializeNode(const Node& node,
                                      std::uint8_t* headPtr) const
{
    // set end of node pointer now before modifying headPtr
    auto varAtPtr = headPtr + node.getSize();

    // build node header
    NodeHeader nodeHeader;
    nodeHeader.setFromNode(node);

    // write node header
    std::memcpy(headPtr, &nodeHeader, sizeof(nodeHeader));
    headPtr += sizeof(nodeHeader);

    // write children
    for (const auto& child : node.getChildren()) {
        ChildNodePointerHeader cnpHeader;
        cnpHeader.setFromChildNodePointer(child);
        std::memcpy(headPtr, &cnpHeader, sizeof(cnpHeader));
        headPtr += sizeof(cnpHeader);
    }

    // serialize intervals
    std::size_t varOffset = 0;
    for (const auto& interval : node.getIntervals()) {
        // update variable data pointer and offset from end
        auto variableDataSize = interval->getVariableDataSize();
        varAtPtr -= variableDataSize;
        varOffset += variableDataSize;

        // build interval header
        IntervalHeader intervalHeader;
        intervalHeader.setFromInterval(*interval);

        // override fixed value with variable data offset if there's any
        if (variableDataSize > 0) {
            intervalHeader.value = static_cast<uint32_t>(varOffset);
        }

        // write interval header
        std::memcpy(headPtr, &intervalHeader, sizeof(intervalHeader));
        headPtr += sizeof(intervalHeader);

        // write variable data
        if (variableDataSize > 0) {
            interval->serializeVariableData(varAtPtr);
        }
    }
}

Node::UP AlignedNodeSerDes::deserializeNode(const std::uint8_t* headPtr,
                                            std::size_t size,
                                            std::size_t maxChildren) const
{
    // set end of node pointer now before modifying headPtr
    auto varEndPtr = headPtr + size;

    // read header
    NodeHeader nodeHeader;
    std::memcpy(&nodeHeader, headPtr, sizeof(nodeHeader));
    headPtr += sizeof(nodeHeader);

    // create node
    auto node = this->createNode(size, maxChildren, nodeHeader.seqNumber,
                                 nodeHeader.parentSeqNumber, nodeHeader.begin);

    // add children
    for (std::size_t x = 0; x < nodeHeader.getChildrenCount(); ++x) {
        // read child node pointer
        ChildNodePointerHeader cnpHeader;
        std::memcpy(&cnpHeader, headPtr, sizeof(cnpHeader));
        headPtr += sizeof(cnpHeader);

        // add child to node
        node->addChild(cnpHeader.begin, cnpHeader.seqNumber);
    }

    // add intervals
    for (std::size_t x = 0; x < nodeHeader.intervalCount; ++x) {
        // read interval header
        IntervalHeader intervalHeader;
        std::memcpy(&intervalHeader, headPtr, sizeof(intervalHeader));
        headPtr += sizeof(intervalHeader);

        // create interval
        auto interval = this->createInterval(intervalHeader.begin,
                                             intervalHeader.end,
                                             intervalHeader.getCatId(),
                                             intervalHeader.getType());

        // deserialize variable data
        auto varDataOffset = static_cast<std::size_t>(interval->getFixedValue());
        auto varAtPtr = varEndPtr - varDataOffset;
        interval->deserializeVariableData(varAtPtr);

        // add interval to node
        AbstractInterval::SP intervalSp {std::move(interval)};
        node->addInterval(intervalSp);
    }

    return node;
}

std::size_t AlignedNodeSerDes::getHeaderSize(const Node& node) const
{
    /* This is a, hopefully temporary, hack to make sure the maximum number
     * of children may be added after the node is full of intervals.
     */
    return sizeof(NodeHeader) +
        node.getMaxChildren() * sizeof(ChildNodePointerHeader);
}

std::size_t AlignedNodeSerDes::getChildNodePointerSize(const ChildNodePointer& cnp) const
{
    return sizeof(ChildNodePointerHeader);
}

std::size_t AlignedNodeSerDes::getIntervalSize(const AbstractInterval& interval) const
{
    return sizeof(IntervalHeader) + interval.getVariableDataSize();
}
