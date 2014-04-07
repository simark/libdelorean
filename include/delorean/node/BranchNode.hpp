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
#ifndef _BRANCHNODE_HPP
#define _BRANCHNODE_HPP

#include <memory>
#include <cstdint>
#include <cstddef>
#include <vector>

#include <delorean/node/AbstractNode.hpp>
#include <delorean/ex/IndexOutOfRange.hpp>
#include <delorean/BasicTypes.hpp>

class BranchNode :
    public AbstractNode
{
public:
    typedef std::shared_ptr<BranchNode> SP;
    typedef std::unique_ptr<BranchNode> UP;
    typedef std::shared_ptr<const BranchNode> CSP;
    typedef std::unique_ptr<const BranchNode> CUP;

protected:
    struct ChildNodePointer
    {
        timestamp_t begin;
        node_seq_t seq;

        enum {
            SIZE = (
                sizeof(begin) +
                sizeof(seq)
            )
        };
    };

public:
    BranchNode(std::size_t size, node_seq_t seqNumber,
               node_seq_t parentSeqNumber, timestamp_t begin,
               unsigned int maxChildren);
    virtual ~BranchNode();

    void addChild(const AbstractNode& childNode);

    node_seq_t getChildSeqAtTs(timestamp_t ts) const;

    node_seq_t getChildSeqAtIndex(unsigned int index) const
    {
        // range check
        if (index >= _children.size()) {
            throw IndexOutOfRange(_children.size(), index);
        }

        return _children[index].seq;
    }

    timestamp_t getChildBeginAtIndex(unsigned int index) const
    {
        // range check
        if (index >= _children.size()) {
            throw IndexOutOfRange(_children.size(), index);
        }

        return _children[index].begin;
    }

    std::size_t getChildrenCount() const
    {
        return _children.size();
    }

    bool isFull() const
    {
        return this->getChildrenCount() >= _maxChildren;
    }

protected:
    std::size_t getSpecificHeaderSize() const;
    void deserializeSpecificHeader(const std::uint8_t* buf);
    void serializeSpecificHeader(std::uint8_t* buf) const;

private:
    // maximum number of children
    unsigned int _maxChildren;

    // children sequence numbers
    std::vector<ChildNodePointer> _children;
};

#endif // _BRANCHNODE_HPP
