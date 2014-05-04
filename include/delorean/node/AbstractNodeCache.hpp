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
#ifndef _ABSTRACTNODECACHE_HPP
#define _ABSTRACTNODECACHE_HPP

#include <cstddef>

#include <delorean/node/Node.hpp>
#include <delorean/BasicTypes.hpp>

namespace delo
{

/**
 * Abstract node cache. Any node cache must inherit this.
 *
 * @author Philippe Proulx
 */
class AbstractNodeCache
{
public:
    typedef std::function<Node::SP (node_seq_t)> GetNodeFromOwnerCb;

public:
    /**
     * Builds an abstract node cache. The \p getNodeFromOwnerCb callback
     * will be used by the cache to get a node from its owner when it
     * cannot be found within the cache.
     *
     * The \p getNodeFromOwnerCb callback must return \a nullptr if
     * the requested node cannot be found.
     *
     * @param size               Size of cache (node count)
     */
    AbstractNodeCache(std::size_t size);

    virtual ~AbstractNodeCache();

    /**
     * Sets the callback used by the node cache to get a node from its
     * owner. The callback must return \a nullptr if the requested node
     * cannot be found.
     *
     * @param getNodeFromOwnerCb Callback to get a node from the cache owner
     */
    void setGetNodeFromOwnerCb(GetNodeFromOwnerCb getNodeFromOwnerCb)
    {
        _getNodeFromOwnerCb = getNodeFromOwnerCb;
    }

    /**
     * Returns a node with sequence number \p seqNumber from the cache.
     *
     * @param seqNumber Sequence number of node to get
     * @returns         Retrieved node or \a nullptr if not found
     */
    Node::SP getNode(node_seq_t seqNumber)
    {
        return this->getNodeImpl(seqNumber);
    }

    /**
     * Checks whether a node is cached or not.
     *
     * @param seqNumber Sequence number of node to check
     * @returns         True if requested node is cached
     */
    bool nodeIsCached(node_seq_t seqNumber) const
    {
        return this->nodeIsCachedImpl(seqNumber);
    }

    /**
     * Invalidates the cache.
     */
    void invalidate()
    {
        this->invalidateImpl();
    }

    /**
     * Returns the size of this cache.
     *
     * @returns Size of this cache (number of nodes)
     */
    std::size_t getSize() const
    {
        return _size;
    }

protected:
    /**
     * A concrete node cache may call this to get a node with sequence
     * number \p seqNumber from its owner. Please note that, depending
     * on the history implementation, this operation could be significantly
     * longer than getting a cached node (well that's the whole point of
     * a cache existence, isn't it?).
     *
     * @param seqNumber Sequence number of node to get from owner
     * @returns         Node or \a nullptr if not found
     */
    Node::SP getNodeFromOwner(node_seq_t seqNumber);

    virtual Node::SP getNodeImpl(node_seq_t seqNumber) = 0;
    virtual bool nodeIsCachedImpl(node_seq_t seqNumber) const = 0;
    virtual void invalidateImpl() = 0;

private:
    // number of nodes that can be contained in this cache
    std::size_t _size;

    // callback to get a node from the cache owner
    GetNodeFromOwnerCb _getNodeFromOwnerCb;
};

}

#endif // _ABSTRACTNODECACHE_HPP
