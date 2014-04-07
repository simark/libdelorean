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
#include <delorean/node/AbstractNode.hpp>
#include <delorean/BasicTypes.hpp>

struct NodeCommonHeader
{
    enum {
        FLAG_DONE_MASK = 1
    };

    // standard node common header fields
    timestamp_t begin;
    timestamp_t end;
    node_type_flags_t typeFlags;
    node_seq_t seqNumber;
    node_seq_t parentSeqNumber;
    uint32_t intervalCount;

    static constexpr node_seq_t ROOT_SEQ_NUMBER()
    {
        return 0xffffffff;
    }

    node_type_t getType() const
    {
        auto type = (typeFlags >> 24) & 0xff;

        return static_cast<node_type_t>(type);
    }

    bool isDone() const
    {
        auto done = typeFlags & FLAG_DONE_MASK;

        return done == FLAG_DONE_MASK;
    }

    void setFromNode(const AbstractNode& node)
    {
        begin = node.getBegin();
        end = node.getEnd();
        seqNumber = node.getSeqNumber();
        parentSeqNumber = node.getParentSeqNumber();
        intervalCount = node.getIntervalCount();

        auto type = static_cast<node_type_t>(node.getType());
        node_flags_t isDone = node.isDone() ? 1 : 0;

        typeFlags = type << 24 | (isDone & FLAG_DONE_MASK);
    }

    enum {
        SIZE = (
            sizeof(begin) +
            sizeof(end) +
            sizeof(typeFlags) +
            sizeof(seqNumber) +
            sizeof(parentSeqNumber) +
            sizeof(intervalCount)
        )
    };
};
