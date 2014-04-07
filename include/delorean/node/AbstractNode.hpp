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
#ifndef _ABSTRACTNODE_HPP
#define _ABSTRACTNODE_HPP

#include <memory>
#include <cstdint>
#include <vector>

#include <delorean/interval/AbstractInterval.hpp>
#include <delorean/interval/IntervalSerializer.hpp>
#include <delorean/interval/IntervalDeserializer.hpp>
#include <delorean/interval/IntervalJar.hpp>
#include <delorean/BasicTypes.hpp>

class AbstractNode
{
public:
    typedef std::shared_ptr<AbstractNode> SP;
    typedef std::unique_ptr<AbstractNode> UP;
    typedef std::shared_ptr<const AbstractNode> CSP;
    typedef std::unique_ptr<const AbstractNode> CUP;

public:
    AbstractNode(std::size_t size, node_seq_t seqNumber,
                 node_seq_t parentSeqNumber, timestamp_t begin,
                 node_type_t type);
    virtual ~AbstractNode();

    void serialize(std::uint8_t* specificHeadPtr,
                   std::uint8_t* varEndPtr,
                   IntervalSerializer& ser) const;
    void deserialize(const std::uint8_t* specificHeadPtr,
                     const std::uint8_t* varEndPtr,
                     std::size_t intervalCount,
                     const IntervalDeserializer& deser);

    void addInterval(AbstractInterval::SP interval);
    bool intervalFits(const AbstractInterval& interval);
    void close(timestamp_t end);

    bool query(timestamp_t ts, IntervalJar& intervals) const;
    AbstractInterval::SP queryFirstMatching(timestamp_t ts,
                                            interval_id_t id) const;

    std::size_t getSize() const
    {
        return _size;
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

    node_type_t getType() const
    {
        return _type;
    }

    std::size_t getIntervalCount() const
    {
        return _intervals.size();
    }

protected:
    virtual std::size_t getSpecificHeaderSize() const = 0;
    virtual void deserializeSpecificHeader(const std::uint8_t* specificHeadPtr) = 0;
    virtual void serializeSpecificHeader(std::uint8_t* specificHeadPtr) const = 0;

    IntervalJar::const_iterator getFirstItForTs(timestamp_t ts) const;

private:
    // node type
    node_type_t _type;

    // size of node (bytes)
    std::size_t _size;

    // time range of this node
    timestamp_t _begin;
    timestamp_t _end;

    // sequence number and parent's sequence number
    node_seq_t _seqNumber;
    node_seq_t _parentSeqNumber;

    // is done
    bool _isDone;

    // total space in bytes taken by all intervals (headers + variable size)
    std::size_t _intervalsSize;

    // jar of intervals
    IntervalJar _intervals;
};

#endif // _ABSTRACTNODE_HPP
