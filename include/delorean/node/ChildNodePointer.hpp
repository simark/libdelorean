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
#ifndef _CHILDNODEPOINTER_HPP
#define _CHILDNODEPOINTER_HPP

#include <delorean/BasicTypes.hpp>

/**
 * Pointer of the child of a node.
 *
 * A node only keeps track of its children using this pointer. The history
 * should know how to find a node in its data structure using only the node's
 * sequence number.
 *
 * @author Philippe Proulx
 */
class ChildNodePointer
{
public:
    /**
     * Builds a child node pointer.
     *
     * @param begin     Begin timestamp
     * @param seqNumber Sequence number
     */
    ChildNodePointer(timestamp_t begin, node_seq_t seqNumber) :
        _begin {begin},
        _seqNumber {seqNumber}
    {
    }

    /**
     * Returns the child node's begin timestamp.
     *
     * @returns Begin timestamp
     */
    timestamp_t getBegin() const
    {
        return _begin;
    }

    /**
     * Returns the child node's sequence number.
     *
     * @returns Sequence number
     */
    node_seq_t getSeqNumber() const
    {
        return _seqNumber;
    }

private:
    timestamp_t _begin;
    node_seq_t _seqNumber;
};

#endif // _CHILDNODEPOINTER_HPP
