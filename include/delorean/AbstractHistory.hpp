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
#ifndef _ABSTRACTHISTORY_HPP
#define _ABSTRACTHISTORY_HPP

#include <delorean/node/Node.hpp>
#include <delorean/interval/AbstractInterval.hpp>
#include <delorean/node/AbstractNodeSerDes.hpp>
#include <delorean/BasicTypes.hpp>

/**
 * Base class for all histories.
 *
 * @author Philippe Proulx
 */
class AbstractHistory
{
public:
    virtual ~AbstractHistory() = 0;

    /**
     * Returns the history begin. Depending on the history implementation,
     * this may not be valid.
     *
     * @returns History begin
     */
    timestamp_t getBegin() const
    {
        return _begin;
    }

    /**
     * Returns the history end. Depending on the history implementation,
     * this may not be valid.
     *
     * @returns History end
     */
    timestamp_t getEnd() const
    {
        return _end;
    }

protected:
    void setBegin(timestamp_t begin)
    {
        _begin = begin;
    }

    void setEnd(timestamp_t end)
    {
        _end = end;
    }

    timestamp_t getLastIntervalEnd() const
    {
        return _lastIntervalEnd;
    }

    void setLastIntervalEnd(timestamp_t end)
    {
        _lastIntervalEnd = end;
    }

    std::size_t getMaxChildren() const
    {
        return _maxChildren;
    }

    void setMaxChildren(std::size_t maxChildren)
    {
        _maxChildren = maxChildren;
    }

    void setNodeSize(std::size_t nodeSize)
    {
        _nodeSize = nodeSize;
    }

    std::size_t getNodeSize() const
    {
        return _nodeSize;
    }

    void setNodeCount(std::size_t nodeCount)
    {
        _nodeCount = nodeCount;
    }

    std::size_t getNodeCount() const
    {
        return _nodeCount;
    }

    void setRootNodeSeqNumber(node_seq_t seq)
    {
        _rootNodeSeqNumber = seq;
    }

    node_seq_t getRootNodeSeqNumber() const
    {
        return _rootNodeSeqNumber;
    }

    bool validateTs(timestamp_t ts)
    {
        return (ts >= _begin && ts <= _end);
    }

    void setNodeSerDes(AbstractNodeSerDes::UP serdes)
    {
        _nodeSerdes = std::move(serdes);
    }

    AbstractNodeSerDes& getNodeSerDes()
    {
        return *_nodeSerdes;
    }

    AbstractNodeSerDes* getNodeSerDesPtr() const
    {
        return _nodeSerdes.get();
    }

    void incNodeCount()
    {
        _nodeCount++;
    }

private:
    timestamp_t _end;
    timestamp_t _lastIntervalEnd;
    timestamp_t _begin;
    std::size_t _maxChildren;
    std::size_t _nodeSize;
    std::size_t _nodeCount;
    node_seq_t _rootNodeSeqNumber;
    AbstractNodeSerDes::UP _nodeSerdes;
};

#endif // _ABSTRACTHISTORY_HPP
