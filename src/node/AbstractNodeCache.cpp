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

#include <delorean/node/AbstractNodeCache.hpp>
#include <delorean/node/Node.hpp>
#include <delorean/BasicTypes.hpp>

namespace delo
{

AbstractNodeCache::AbstractNodeCache(std::size_t size) :
    _size {size}
{
}

AbstractNodeCache::~AbstractNodeCache()
{
}

Node::SP AbstractNodeCache::getNodeFromOwner(node_seq_t seqNumber)
{
    if (!_getNodeFromOwnerCb) {
        return nullptr;
    }

    return _getNodeFromOwnerCb(seqNumber);
}

}
