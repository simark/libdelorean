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
#ifndef _ABSTRACTHISTORY_HPP
#define _ABSTRACTHISTORY_HPP

#include <delorean/node/AbstractNode.hpp>
#include <delorean/interval/AbstractInterval.hpp>
#include <delorean/BasicTypes.hpp>

class AbstractHistory
{
public:
    virtual ~AbstractHistory() = 0;

protected:
    timestamp_t getBegin() const
    {
        return _begin;
    }

    void setBegin(timestamp_t begin)
    {
        _begin = begin;
    }

    timestamp_t getEnd() const
    {
        return _end;
    }

    void setEnd(timestamp_t end)
    {
        _end = end;
    }

    unsigned int getMaxChildren() const
    {
        return _maxChildren;
    }

    void setMaxChildren(unsigned int maxChildren)
    {
        _maxChildren = maxChildren;
    }

    void setNodeSize(std::size_t nodeSize)
    {
        _nodeSize = nodeSize;
    }

    unsigned int getNodeSize() const
    {
        return _nodeSize;
    }

    void setNodeCount(unsigned int nodeCount)
    {
        _nodeCount = nodeCount;
    }

    unsigned int getNodeCount() const
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

    std::vector<AbstractNode::SP>& getLatestBranch()
    {
        return _latestBranch;
    }

private:
    timestamp_t _end;
    timestamp_t _begin;
    unsigned int _maxChildren;
    std::size_t _nodeSize;
    unsigned int _nodeCount;
    node_seq_t _rootNodeSeqNumber;
    std::vector<AbstractNode::SP> _latestBranch;
};

#endif // _ABSTRACTHISTORY_HPP
