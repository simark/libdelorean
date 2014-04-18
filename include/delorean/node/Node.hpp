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
#ifndef _NODE_HPP
#define _NODE_HPP

#include <memory>
#include <cstdint>
#include <vector>

#include <delorean/interval/AbstractInterval.hpp>
#include <delorean/interval/IntervalJar.hpp>
#include <delorean/node/AbstractNodeSerDes.hpp>
#include <delorean/ex/IndexOutOfRange.hpp>
#include <delorean/BasicTypes.hpp>

class Node
{
public:
    typedef std::shared_ptr<Node> SP;
    typedef std::unique_ptr<Node> UP;
    typedef std::shared_ptr<const Node> CSP;
    typedef std::unique_ptr<const Node> CUP;

public:
    Node(std::size_t size, std::size_t maxChildren, node_seq_t seqNumber,
         node_seq_t parentSeqNumber, timestamp_t begin,
         const AbstractNodeSerDes* serdes);
    virtual ~Node();

    void addInterval(AbstractInterval::SP interval);
    bool intervalFits(const AbstractInterval& interval);
    void close(timestamp_t end);

    bool query(timestamp_t ts, IntervalJar& intervals) const;
    AbstractInterval::SP queryFirstMatching(timestamp_t ts,
                                            interval_id_t id) const;

    void addChild(timestamp_t begin, node_seq_t seqNumber);
    node_seq_t getChildSeqAtTs(timestamp_t ts) const;

    node_seq_t getChildSeqAtIndex(unsigned int index) const
    {
        // range check
        if (index >= _children.size()) {
            throw IndexOutOfRange(_children.size(), index);
        }

        return _children.at(index).getSeqNumber();
    }

    timestamp_t getChildBeginAtIndex(unsigned int index) const
    {
        // range check
        if (index >= _children.size()) {
            throw IndexOutOfRange(_children.size(), index);
        }

        return _children.at(index).getBegin();
    }

    std::size_t getChildrenCount() const
    {
        return _children.size();
    }

    bool isFull() const
    {
        return this->getChildrenCount() >= _maxChildren;
    }

    std::size_t getSize() const
    {
        return _totalSize;
    }

    timestamp_t getBegin() const
    {
        return _begin;
    }

    timestamp_t getEnd() const
    {
        return _end;
    }

    node_seq_t getSeqNumber() const
    {
        return _seqNumber;
    }

    node_seq_t getParentSeqNumber() const
    {
        return _parentSeqNumber;
    }

    void setParentSeqNumber(node_seq_t parentSeqNumber)
    {
        _parentSeqNumber = parentSeqNumber;
    }

    bool isDone() const
    {
        return _isDone;
    }

    bool isExtended() const
    {
        return _isExtended;
    }

    std::size_t getIntervalCount() const
    {
        return _intervals.size();
    }

    const IntervalJar& getIntervalJar() const
    {
        return _intervals;
    }

    std::size_t getMaxChildren() const
    {
        return _maxChildren;
    }

    const IntervalJar& getIntervals() const
    {
        return _intervals;
    }

    const std::vector<ChildNodePointer> getChildren() const
    {
        return _children;
    }

    static constexpr node_seq_t ROOT_SEQ_NUMBER()
    {
        return 0xffffffff;
    }

private:
    IntervalJar::const_iterator getFirstItForTs(timestamp_t ts) const;
    void computeHeaderSize();

private:
    // time range of this node
    timestamp_t _begin;
    timestamp_t _end;

    // sequence number and parent's sequence number
    node_seq_t _seqNumber;
    node_seq_t _parentSeqNumber;

    // flags
    bool _isDone;
    bool _isExtended;

    // jar of intervals
    IntervalJar _intervals;

    // maximum number of children
    std::size_t _maxChildren;

    // current sizes in bytes
    std::size_t _curHeaderSize;
    std::size_t _curChildrenSize;
    std::size_t _curIntervalsSize;

    // total size
    std::size_t _totalSize;

    // children sequence numbers
    std::vector<ChildNodePointer> _children;

    // serializer/deserializer
    const AbstractNodeSerDes* _serdes;
};

#endif // _NODE_HPP
