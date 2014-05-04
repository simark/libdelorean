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
#include <cstddef>
#include <stdexcept>
#include <iostream>

#include <delorean/node/AbstractNodeCache.hpp>
#include <delorean/node/LRUNodeCache.hpp>
#include <delorean/node/Node.hpp>
#include <delorean/BasicTypes.hpp>

namespace delo {

LRUNodeCache::LRUNodeCache(std::size_t size) :
    AbstractNodeCache {size}
{

}

Node::SP LRUNodeCache::getNodeImpl(node_seq_t seqNumber)
{
    nodes_map_t::iterator it;
    Node::SP node;

    it = nm.find(seqNumber);

    if (it != nm.end()) {
        /* Item is present in cache, put it back in front */
        lru_list_t::iterator list_it = it->second;
        node = *list_it;
        lru.erase(list_it);
        lru.push_front(node);
    } else {
        /* Item is not in cache */
        node = this->getNodeFromOwner(seqNumber);
        if (node != nullptr) {
            lru.push_front(node);
            lru_list_t::iterator newNodeIt = lru.begin();
            nm[seqNumber] = newNodeIt;

            if (lru.size() > getSize()) {
                Node::SP droppedNode = lru.back();
                lru.pop_back();

                node_seq_t droppedSeqNumber = droppedNode->getSeqNumber();
                nm.erase(droppedSeqNumber);
            }
        }
    }

    return node;
}

bool LRUNodeCache::nodeIsCachedImpl(node_seq_t seqNumber) const
{
    nodes_map_t::const_iterator it;

    it = nm.find(seqNumber);

    return it != nm.end();
}

void LRUNodeCache::invalidateImpl()
{
    lru.clear();
    nm.clear();
}

} // namespace delo
