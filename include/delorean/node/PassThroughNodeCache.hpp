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
#ifndef _PASSTHROUGHNODECACHE_HPP
#define _PASSTHROUGHNODECACHE_HPP

#include <cstddef>

#include <delorean/node/AbstractNodeCache.hpp>
#include <delorean/node/Node.hpp>
#include <delorean/BasicTypes.hpp>

/**
 * Pass-through node cache. This effectively means no cache.
 *
 * @author Philippe Proulx
 */
class PassThroughNodeCache :
    public AbstractNodeCache
{
public:
    /**
     * Builds a direct mapped node cache. \p size must be a power of two.
     *
     * @param size      Size of cache (node count, power of two)
     * @param getNodeCb Function called to get a node from the cache owner
     */
    PassThroughNodeCache(std::size_t size, GetNodeFromOwnerCb getNodeFromOwnerCb) :
        AbstractNodeCache {size, getNodeFromOwnerCb}
    {
    }

protected:
    Node::SP getNodeImpl(node_seq_t seqNumber)
    {
        return this->getNodeFromOwner(seqNumber);
    }

    bool nodeIsCachedImpl(node_seq_t seqNumber) const
    {
        return false;
    }

    void invalidateImpl()
    {
    }
};

#endif // _PASSTHROUGHNODECACHE_HPP
