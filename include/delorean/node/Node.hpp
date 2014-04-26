/* Copyright (c) 2014 Philippe Proulx <eepp.ca>
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

/**
 * A history tree node.
 *
 * A node is a container of both pointers to other nodes (parent and
 * children) and intervals ordered by ascending end time.
 *
 * A node keeps a pointer to a specific node serializer/deserializer
 * because, in order to know if the next interval will fit, it has to know
 * the sizes of its header, its children pointers and its intervals. Those
 * sizes are not the same with all node ser/des.
 *
 * @author Philippe Proulx
 */
class Node
{
public:
    /// Shared pointer to a node
    typedef std::shared_ptr<Node> SP;

    /// Unique pointer to a node
    typedef std::unique_ptr<Node> UP;

public:
    /**
     * Builds a node.
     *
     * @param size            Node total size
     * @param maxChildren     Maximum number of children to allow
     * @param parentSeqNumber Sequence number of parent
     * @param begin           Begin timestamp
     * @param serdes          Node ser/des pointer
     */
    Node(std::size_t size, std::size_t maxChildren, node_seq_t seqNumber,
         node_seq_t parentSeqNumber, timestamp_t begin,
         const AbstractNodeSerDes* serdes);

    virtual ~Node();

    /**
     * Adds an interval to this node.
     *
     * @param interval Interval to add
     */
    void addInterval(AbstractInterval::SP interval);

    /**
     * Returns whether the interval \p interval fits in this node or not.
     *
     * @param interval Interval that must be checked
     * @returns        True if the interval fits
     */
    bool intervalFits(const AbstractInterval& interval);

    /**
     * Closes this node with end timestamp \p end. Once a node is closed,
     * it's not possible to add new intervals or new children.
     *
     * @param end End timestamp with which to close the node
     */
    void close(timestamp_t end);

    /**
     * Finds all intervals that intersect with \p ts.
     *
     * @param ts        Timestamp
     * @param intervals Jar in which to add matching intervals
     * @returns         True if at least one interval was found
     */
    bool findAll(timestamp_t ts, IntervalJar& intervals) const;

    /**
     * Finds the first interval intersecting \p ts and having key
     * \p key.
     *
     * @param ts    Timestamp
     * @param key   Matching key
     * @returns     Marching interval or \a nullptr if nothing found
     */
    AbstractInterval::SP findOne(timestamp_t ts, interval_key_t key) const;

    /**
     * Adds a child (pointer) to this node.
     *
     * @param begin     Begin timestamp of child to add
     * @param seqNumber Sequence number of child to add
     */
    void addChild(timestamp_t begin, node_seq_t seqNumber);

    /**
     * Returns the child sequence number including timestamp \p ts. The
     * returned sequence number may be this node's sequence number. Caller
     * must make sure the timestamp is at least within this node's
     * boundaries.
     *
     * @param ts Timestamp
     * @returns  Sequence number including timestamp
     */
    node_seq_t getChildSeqAtTs(timestamp_t ts) const;

    /**
     * Returns the child sequence number at index \p index.
     *
     * @param index Index of child to get
     * @returns     Child sequence number
     */
    node_seq_t getChildSeqAtIndex(std::size_t index) const
    {
        // range check
        if (index >= _children.size()) {
            throw IndexOutOfRange(_children.size(), index);
        }

        return _children.at(index).getSeqNumber();
    }

    /**
     * Returns the child begin timestamp at index \p index.
     *
     * @param index Index of child to get
     * @param       Child begin timestamp
     */
    timestamp_t getChildBeginAtIndex(std::size_t index) const
    {
        // range check
        if (index >= _children.size()) {
            throw IndexOutOfRange(_children.size(), index);
        }

        return _children.at(index).getBegin();
    }

    /**
     * Returns the number of children in this node.
     *
     * @returns Number of children in this node
     */
    std::size_t getChildrenCount() const
    {
        return _children.size();
    }

    /**
     * Returns whether this node is full (of children) or not.
     *
     * @returns True of this node is full (of children)
     */
    bool isFull() const
    {
        return this->getChildrenCount() >= _maxChildren;
    }

    /**
     * Returns this node's total size.
     *
     * @returns Total size
     */
    std::size_t getSize() const
    {
        return _totalSize;
    }

    /**
     * Returns this node's begin timestamp.
     *
     * @returns Begin timestamp.
     */
    timestamp_t getBegin() const
    {
        return _begin;
    }

    /**
     * Returns this node's end timestamp.
     *
     * @returns End timestamp
     */
    timestamp_t getEnd() const
    {
        return _end;
    }

    /**
     * Returns this node's sequence number.
     *
     * @returns Sequence number
     */
    node_seq_t getSeqNumber() const
    {
        return _seqNumber;
    }

    /**
     * Returns this node's parent sequence number. This will be the constant
     * returned by ROOT_PARENT_SEQ_NUMBER() if this node is the root.
     *
     * @returns Parent sequence number.
     */
    node_seq_t getParentSeqNumber() const
    {
        return _parentSeqNumber;
    }

    /**
     * Sets the current parent sequence number.
     *
     * @param parentSeqNumber Parent sequence number
     */
    void setParentSeqNumber(node_seq_t parentSeqNumber)
    {
        _parentSeqNumber = parentSeqNumber;
    }

    /**
     * Returns whether this node is done (finished) or not.
     *
     * @returns True if this node is done
     */
    bool isDone() const
    {
        return _isDone;
    }

    /**
     * Returns whether this node is extended or not.
     *
     * @returns True if this node is extended
     */
    bool isExtended() const
    {
        return _isExtended;
    }

    /**
     * Returns the current number of intervals.
     *
     * @returns Current number of intervals
     */
    std::size_t getIntervalCount() const
    {
        return _intervals.size();
    }

    /**
     * Returns a reference to the jar of intervals.
     *
     * @returns Interval jar reference
     */
    const IntervalJar& getIntervals() const
    {
        return _intervals;
    }

    /**
     * Returns this node's maximum number of children.
     *
     * @returns Maximum number of children
     */
    std::size_t getMaxChildren() const
    {
        return _maxChildren;
    }

    /**
     * Returns a reference to all child node pointers of this node.
     *
     * @returns Reference to child node pointers
     */
    const std::vector<ChildNodePointer>& getChildren() const
    {
        return _children;
    }

    /**
     * Returns the root node's parent sequence number constant.
     *
     * @returns Root node's parent sequence number
     */
    static constexpr node_seq_t ROOT_PARENT_SEQ_NUMBER()
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
