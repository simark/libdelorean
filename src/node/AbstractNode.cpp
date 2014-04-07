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

#include <delorean/interval/IntervalJar.hpp>
#include <delorean/interval/IntervalSerializer.hpp>
#include <delorean/interval/IntervalDeserializer.hpp>
#include <delorean/node/AbstractNode.hpp>
#include <delorean/ex/TimestampOutOfRange.hpp>
#include <delorean/BasicTypes.hpp>
#include "NodeStructs.hpp"

AbstractNode::AbstractNode(std::size_t size, node_seq_t seqNumber,
                           node_seq_t parentSeqNumber, timestamp_t begin,
                           node_type_t type) :
    _type {type},
    _size {size},
    _begin {begin},
    _end {begin},
    _seqNumber {seqNumber},
    _parentSeqNumber {parentSeqNumber},
    _isDone {false},
    _intervalsSize {0}
{
}

AbstractNode::~AbstractNode()
{
}

void AbstractNode::serialize(std::uint8_t* specificHeadPtr,
                             std::uint8_t* varEndPtr,
                             IntervalSerializer& ser) const
{
    // serialize specific header
    this->serializeSpecificHeader(specificHeadPtr);
    specificHeadPtr += this->getSpecificHeaderSize();

    // serialize intervals
    std::size_t varOffset = 0;
    for (const auto& interval : _intervals) {
        varEndPtr -= interval->getVariableSize();
        varOffset += interval->getVariableSize();
        ser.serializeInterval(*interval, specificHeadPtr, varEndPtr, varOffset);
        specificHeadPtr += interval->getHeaderSize();
    }
}

void AbstractNode::deserialize(const std::uint8_t* specificHeadPtr,
                               const std::uint8_t* varEndPtr,
                               std::size_t intervalCount,
                               const IntervalDeserializer& deser)
{
    // deserialize specific header
    this->deserializeSpecificHeader(specificHeadPtr);
    specificHeadPtr += this->getSpecificHeaderSize();

    // deserialize intervals
    for (unsigned int x = 0; x < intervalCount; ++x) {
        auto interval = std::move(deser.deserializeInterval(specificHeadPtr, varEndPtr));

        this->addInterval(std::move(interval));
    }

    // mark this node done
    _isDone = true;
}

void AbstractNode::addInterval(AbstractInterval::SP interval)
{
    if (!_intervals.empty()) {
        // make sure the insertion is in ascending order of end time
        if (interval->getEnd() < _end) {
            throw TimestampOutOfRange {_begin, _end, interval->getEnd()};
        }
    }

    // add interval to jar
    _intervals.push_back(interval);

    // update size cache
    _intervalsSize += interval->getSize();

    // update node's end time
    _end = interval->getEnd();
}

bool AbstractNode::intervalFits(const AbstractInterval& interval)
{
    auto freeSpace = _size - NodeCommonHeader::SIZE -
        this->getSpecificHeaderSize() - _intervalsSize;

    return interval.getSize() <= freeSpace;
}

void AbstractNode::close(timestamp_t end)
{
    // already closed?
    if (_isDone) {
        return;
    }

    // range check
    if (end < _end) {
        throw TimestampOutOfRange {_begin, _end, end};
    }

	_isDone = true;
	_end = end;
}

bool AbstractNode::query(timestamp_t ts, IntervalJar& intervals) const
{
    // fast path when there's no interval
    if (_intervals.empty()) {
        return false;
    }

    // range check
    if (ts < _begin || ts > _end) {
        throw TimestampOutOfRange {_begin, _end, ts};
    }

    auto found = false;
    for (auto it = getFirstItForTs(ts); it != _intervals.end(); it++) {
        auto interval = *it;

        if (interval->getBegin() <= ts) {
            intervals.push_back(interval);
            found = true;
        }
    }

    return found;
}

AbstractInterval::SP AbstractNode::queryFirstMatching(timestamp_t ts,
                                                      interval_id_t id) const
{
    // fast path when there's no interval
    if (_intervals.empty()) {
        return nullptr;
    }

    // range check
    if (ts < _begin || ts > _end) {
        throw TimestampOutOfRange {_begin, _end, ts};
    }

    for (auto it = getFirstItForTs(ts); it != _intervals.end(); it++) {
        auto interval = *it;

        if (interval->getId() == id && interval->getBegin() <= ts) {
            return interval;
        }
    }

    return nullptr;
}

IntervalJar::const_iterator AbstractNode::getFirstItForTs(timestamp_t ts) const
{
    /* Here we know that intervals are already sorted in ascending order of
     * end time because that's like the sole requirement of this whole thing.
     * Therefore we can perform a binary search.
     *
     * So we have something like this:
     *
     *      |                                           ¦
     *      [==========]                                ¦
     *      ¦  [==========]                             ¦
     *      ¦      [==========]                         ¦
     *      ¦    [===============]                      ¦
     *      ¦           [====================]          ¦
     *      ¦        [=======================]          ¦
     *      ¦                [==================]       ¦
     *      ¦      [====================================]
     *      ¦                                           ¦
     *     node                                       node
     *     begin                                       end
     *
     * If everything went well, `ts` should be within the node range, so we
     * won't care checking for this again. We want the first interval whose
     * end time is NOT LESS THAN `ts`.
     */
    auto compare = [] (const AbstractInterval::SP& a, const timestamp_t& b)
    {
        return a->getEnd() < b;
    };

    /* This will return `_intervals.end()` if nothing is found, although if
     * we're here, it should always find something.
     */
    auto it = std::lower_bound(_intervals.begin(), _intervals.end(),
                               ts, compare);

    return it;
}
