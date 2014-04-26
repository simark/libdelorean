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
#include <algorithm>

#include <delorean/node/Node.hpp>
#include <delorean/interval/IntervalJar.hpp>
#include <delorean/ex/TimestampOutOfRange.hpp>
#include <delorean/ex/NodeFull.hpp>
#include <delorean/BasicTypes.hpp>

Node::Node(std::size_t size, std::size_t maxChildren, node_seq_t seqNumber,
           node_seq_t parentSeqNumber, timestamp_t begin,
           const AbstractNodeSerDes* serdes) :
    _begin {begin},
    _end {begin},
    _seqNumber {seqNumber},
    _parentSeqNumber {parentSeqNumber},
    _isDone {false},
    _isExtended {false},
    _maxChildren {maxChildren},
    _curHeaderSize {0},
    _curChildrenSize {0},
    _curIntervalsSize {0},
    _totalSize {size},
    _serdes {serdes}
{
    this->computeHeaderSize();
}

Node::~Node()
{
}

void Node::computeHeaderSize()
{
    _curHeaderSize = _serdes->getHeaderSize(*this);
}

void Node::addInterval(AbstractInterval::SP interval)
{
    /* We don't perform any range check here. Since a node is not exposed
     * to libdelorean's end user, the history should perform that check
     * only once.
     */

    // add interval to jar
    _intervals.push_back(interval);

    // update size cache
    _curIntervalsSize += _serdes->getIntervalSize(*interval);

    // update node's end time
    _end = interval->getEnd();

    // end changed: recompute header size
    this->computeHeaderSize();
}

bool Node::intervalFits(const AbstractInterval& interval)
{
    auto curSize = _curHeaderSize + _curChildrenSize + _curIntervalsSize;
    auto freeSpace = _totalSize - curSize;
    auto intervalSize = _serdes->getIntervalSize(interval);

    return intervalSize <= freeSpace;
}

void Node::close(timestamp_t end)
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

    // end changed: recompute header size
    this->computeHeaderSize();
}

bool Node::findAll(timestamp_t ts, IntervalJar& intervals) const
{
    /* We don't perform any range check here. Since a node is not exposed
     * to libdelorean's end user, the history should perform that check
     * only once.
     */

    // fast path when there's no interval
    if (_intervals.empty()) {
        return false;
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

AbstractInterval::SP Node::findOne(timestamp_t ts, interval_key_t key) const
{
    /* We don't perform any range check here. Since a node is not exposed
     * to libdelorean's end user, the history should perform that check
     * only once.
     */

    // fast path when there's no interval
    if (_intervals.empty()) {
        return nullptr;
    }

    for (auto it = getFirstItForTs(ts); it != _intervals.end(); it++) {
        auto interval = *it;

        if (interval->getKey() == key && interval->getBegin() <= ts) {
            return interval;
        }
    }

    return nullptr;
}

IntervalJar::const_iterator Node::getFirstItForTs(timestamp_t ts) const
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

void Node::addChild(timestamp_t begin, node_seq_t seqNumber)
{
    // node full?
    if (this->isFull()) {
        throw NodeFull();
    }

    // add to list
    _children.push_back(ChildNodePointer {begin, seqNumber});

    // child added: update children size
    _curChildrenSize += _serdes->getChildNodePointerSize(_children.back());
}

node_seq_t Node::getChildSeqAtTs(timestamp_t ts) const
{
	auto potentialNextSeqNumber = this->getSeqNumber();

    for (const auto& child : _children) {
        if (ts >= child.getBegin()) {
            potentialNextSeqNumber = child.getSeqNumber();
        } else {
            break;
        }
    }

	return potentialNextSeqNumber;
}
