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
#ifndef _ALIGNEDNODEDESER_HPP
#define _ALIGNEDNODEDESER_HPP

#include <cstdint>

#include <delorean/node/Node.hpp>
#include <delorean/node/ChildNodePointer.hpp>
#include <delorean/node/AbstractNodeSerDes.hpp>
#include <delorean/BasicTypes.hpp>

class AlignedNodeSerDes :
    public AbstractNodeSerDes
{
public:
    AlignedNodeSerDes();
    virtual ~AlignedNodeSerDes();

    // serializer side
    void serializeNode(const Node& node, std::uint8_t* headPtr) const;
    std::size_t getHeaderSize(const Node& node) const;
    std::size_t getChildNodePointerSize(const ChildNodePointer& cnp) const;
    std::size_t getIntervalSize(const AbstractInterval& interval) const;

    // deserializer side
    Node::UP deserializeNode(const std::uint8_t* headPtr,
                             std::size_t size,
                             unsigned int maxChildren) const;

private:
    struct NodeHeader
    {
        enum {
            FLAG_DONE_MASK = 1,
            FLAG_EXTENDED_MASK = 2,
        };

        // standard node common header fields
        timestamp_t begin;
        timestamp_t end;
        std::uint32_t childrenCountFlags;
        node_seq_t seqNumber;
        node_seq_t parentSeqNumber;
        uint32_t intervalCount;

        std::size_t getChildrenCount() const
        {
            auto childrenCount = (childrenCountFlags >> 8) & 0xffffff;

            return static_cast<std::size_t>(childrenCount);
        }

        bool isDone() const
        {
            auto done = childrenCountFlags & FLAG_DONE_MASK;

            return done == FLAG_DONE_MASK;
        }

        bool isExtended() const
        {
            auto extended = childrenCountFlags & FLAG_EXTENDED_MASK;

            return extended == FLAG_EXTENDED_MASK;
        }

        void setFromNode(const Node& node)
        {
            begin = node.getBegin();
            end = node.getEnd();
            seqNumber = node.getSeqNumber();
            parentSeqNumber = node.getParentSeqNumber();
            intervalCount = node.getIntervalCount();

            uint32_t childrenCount = static_cast<uint32_t>(node.getChildrenCount());
            childrenCount <<= 8;
            uint32_t isDone = node.isDone() ? FLAG_DONE_MASK : 0;
            uint32_t isExtended = node.isExtended() ? FLAG_EXTENDED_MASK : 0;

            childrenCountFlags = childrenCount | isDone | isExtended;
        }
    };

    struct IntervalHeader
    {
        // standard interval header fields
        timestamp_t begin;
        timestamp_t end;
        uint32_t typeId;
        interval_value_t value;

        interval_type_t getType() const
        {
            auto type = (typeId >> 24) & 0xff;

            return static_cast<interval_type_t>(type);
        }

        interval_id_t getId() const
        {
            auto id = typeId & 0xffffff;

            return static_cast<interval_id_t>(id);
        }

        void setFromInterval(const AbstractInterval& interval)
        {
            begin = interval.getBegin();
            end = interval.getEnd();
            value = interval.getFixedValue();

            auto type = static_cast<uint32_t>(interval.getType());
            type <<= 24;
            auto id = static_cast<uint32_t>(interval.getId());
            id &= 0xffffff;

            typeId = type | id;
        }
    };

    struct ChildNodePointerHeader
    {
        timestamp_t begin;
        node_seq_t seqNumber;

        void setFromChildNodePointer(const ChildNodePointer& cnp)
        {
            begin = cnp.getBegin();
            seqNumber = cnp.getSeqNumber();
        }
    };
};

#endif // _ALIGNEDNODEDESER_HPP
