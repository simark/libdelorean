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
#include <memory>
#include <cstddef>
#include <stdexcept>

#include <delorean/node/Node.hpp>
#include <delorean/node/AlignedNodeSerDes.hpp>
#include <delorean/node/LruNodeCache.hpp>
#include <delorean/BasicTypes.hpp>
#include "LruNodeCacheTest.hpp"

using namespace delo;

CPPUNIT_TEST_SUITE_REGISTRATION(LruNodeCacheTest);

namespace
{

void checkCached(LruNodeCache& cache, std::vector<bool> expectedCached)
{
    for (auto i = 0U; i < expectedCached.size(); i++) {
        auto isCached = cache.nodeIsCached(i);
        bool expectedIsCached = expectedCached[i];

        CPPUNIT_ASSERT_EQUAL(expectedIsCached, isCached);
    }
}

void getAndCheckNode(LruNodeCache& cache, node_seq_t seqNumber,
                     std::map<node_seq_t, Node::SP>& nodes)
{
    auto receivedNode = cache.getNode(seqNumber);
    Node::SP expectedNode = nullptr;
    auto it = nodes.find(seqNumber);

    if (it != nodes.end()) {
        expectedNode = it->second;
    }

    CPPUNIT_ASSERT_EQUAL(expectedNode, receivedNode);
}

}

void LruNodeCacheTest::testConstructorAndAttributes()
{
    // test creation
    LruNodeCache cache {10};

    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(10), cache.getSize());
}

void LruNodeCacheTest::testGetNode()
{
    AlignedNodeSerDes serdes;
    Node::SP node;

    std::map<node_seq_t, Node::SP> nodes = {
        {1, Node::SP {new Node {1024, 4, 1, 0, 0, &serdes}}},
        {2, Node::SP {new Node {1024, 4, 2, 0, 0, &serdes}}},
        {4, Node::SP {new Node {1024, 4, 4, 0, 0, &serdes}}},
        {5, Node::SP {new Node {1024, 4, 5, 0, 0, &serdes}}},
        {6, Node::SP {new Node {1024, 4, 6, 0, 0, &serdes}}},
        {8, Node::SP {new Node {1024, 4, 8, 0, 0, &serdes}}},
        {9, Node::SP {new Node {1024, 4, 9, 0, 0, &serdes}}},
    };

    // build cache
    LruNodeCache cache {4};
    cache.setGetNodeFromOwnerCb([&] (node_seq_t seqNumber) -> Node::SP {
        auto it = nodes.find(seqNumber);

        if (it == nodes.end()) {
            return nullptr;
        }

        return it->second;
    });

    checkCached(cache, {false, false, false, false, false, false, false, false, false, false});
    getAndCheckNode(cache, 5, nodes);
    checkCached(cache, {false, false, false, false, false, true,  false, false, false, false});
    getAndCheckNode(cache, 3, nodes);
    checkCached(cache, {false, false, false, false, false, true, false, false, false, false});
    getAndCheckNode(cache, 1, nodes);
    checkCached(cache, {false, true,  false, false, false, true, false, false, false, false});
    getAndCheckNode(cache, 4, nodes);
    checkCached(cache, {false, true, false, false, true, true, false, false, false, false});
    getAndCheckNode(cache, 9, nodes);
    checkCached(cache, {false, true, false, false, true, true, false, false, false, true});
    getAndCheckNode(cache, 9, nodes);
    checkCached(cache, {false, true, false, false, true, true, false, false, false, true});
    getAndCheckNode(cache, 6, nodes);
    checkCached(cache, {false, true, false, false, true, false, true, false, false, true});
    getAndCheckNode(cache, 5, nodes);
    checkCached(cache, {false, false, false, false, true, true, true, false, false, true});
    getAndCheckNode(cache, 8, nodes);
    checkCached(cache, {false, false, false, false, false, true, true, false, true, true});
    getAndCheckNode(cache, 7, nodes);
    checkCached(cache, {false, false, false, false, false, true, true, false, true, true});
    getAndCheckNode(cache, 9, nodes);
    checkCached(cache, {false, false, false, false, false, true, true, false, true, true});
    getAndCheckNode(cache, 4, nodes);
    checkCached(cache, {false, false, false, false, true, true, false, false, true, true});
    getAndCheckNode(cache, 4, nodes);
    checkCached(cache, {false, false, false, false, true, true, false, false, true, true});
    getAndCheckNode(cache, 8, nodes);
    checkCached(cache, {false, false, false, false, true, true, false, false, true, true});
    getAndCheckNode(cache, 1, nodes);
    checkCached(cache, {false, true, false, false, true, false, false, false, true, true});
}
