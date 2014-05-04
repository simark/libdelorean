/* Copyright (c) 2014 Philippe Proulx <eepp.ca>
 *               2014 Simon Marchi <ppee.ca>
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
#ifndef _LRUNODECACHE_HPP
#define _LRUNODECACHE_HPP

#include <cstddef>
#include <list>
#include <map>

#include <delorean/node/AbstractNodeCache.hpp>
#include <delorean/node/Node.hpp>
#include <delorean/BasicTypes.hpp>

namespace delo {

/**
 * Least recently used node cache. The cache always replaces the least
 * recently used node on a cache miss.
 *
 * @author Simon Marchi
 */
class LruNodeCache :
    public AbstractNodeCache
{
private:
    typedef std::list<Node::SP> NodeList;
    typedef std::map<node_seq_t, NodeList::iterator> NodeMap;

public:
    /**
     * Builds an LRU node cache.
     *
     * @param size Size of cache (node count)
     */
    LruNodeCache(std::size_t size);

protected:
    Node::SP getNodeImpl(node_seq_t seqNumber);
    bool nodeIsCachedImpl(node_seq_t seqNumber) const;
    void invalidateImpl();

private:
    NodeList _list;
    NodeMap _map;
};

}

#endif // _LRUNODECACHE_HPP
