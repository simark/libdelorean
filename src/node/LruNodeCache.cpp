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
#include <delorean/node/LruNodeCache.hpp>
#include <delorean/node/Node.hpp>
#include <delorean/BasicTypes.hpp>

namespace delo {

LruNodeCache::LruNodeCache(std::size_t size) :
    AbstractNodeCache {size}
{
}

Node::SP LruNodeCache::getNodeImpl(node_seq_t seqNumber)
{
    Node::SP node;

    // try finding the requested node in our map
    auto it = _map.find(seqNumber);

    if (it != _map.end()) {
        // hit: put it back in front
        auto listIt = it->second;
        node = *listIt;
        _list.erase(listIt);
        _list.push_front(node);
    } else {
        // miss
        node = this->getNodeFromOwner(seqNumber);
        if (node) {
            _list.push_front(node);
            auto newNodeIt = _list.begin();
            _map[seqNumber] = newNodeIt;

            if (_list.size() > this->getSize()) {
                auto droppedNode = _list.back();
                _list.pop_back();

                auto droppedSeqNumber = droppedNode->getSeqNumber();
                _map.erase(droppedSeqNumber);
            }
        }
    }

    return node;
}

bool LruNodeCache::nodeIsCachedImpl(node_seq_t seqNumber) const
{
    auto it = _map.find(seqNumber);

    return it != _map.end();
}

void LruNodeCache::invalidateImpl()
{
    _list.clear();
    _map.clear();
}

}
