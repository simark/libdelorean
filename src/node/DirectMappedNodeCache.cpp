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
#include <cstddef>
#include <stdexcept>

#include <delorean/node/AbstractNodeCache.hpp>
#include <delorean/node/DirectMappedNodeCache.hpp>
#include <delorean/node/Node.hpp>
#include <delorean/BasicTypes.hpp>

DirectMappedNodeCache::DirectMappedNodeCache(std::size_t size) :
    AbstractNodeCache {size}
{
    // power of two?
    if ((size & (size - 1)) != 0) {
        throw std::invalid_argument {
            "Direct mapped node cache size must be a power of two"
        };
    }

    // resize cache vector now
    _cache.resize(size);
}

Node::SP DirectMappedNodeCache::getNodeImpl(node_seq_t seqNumber)
{
    // cache position for sequence number
    auto pos = cachePosForSeqNumber(seqNumber);

    // put in cache if necessary
    Node::SP node;
    if (!this->nodeIsCached(seqNumber)) {
        node = this->getNodeFromOwner(seqNumber);
        if (node != nullptr) {
            /* Do not overwrite something else in cache if the node
             * doesn't even exist.
             */
            _cache.at(pos) = node;
        }
    } else {
        node = _cache.at(pos);
    }

    return node;
}

bool DirectMappedNodeCache::nodeIsCachedImpl(node_seq_t seqNumber) const
{
    const auto& node = _cache.at(this->cachePosForSeqNumber(seqNumber));

    if (node == nullptr) {
        return false;
    }

    return node->getSeqNumber() == seqNumber;
}

void DirectMappedNodeCache::invalidateImpl()
{
    for (auto& node : _cache) {
        node = nullptr;
    }
}
