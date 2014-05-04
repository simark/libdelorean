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
#include <memory>
#include <cstddef>
#include <stdexcept>

#include <delorean/node/Node.hpp>
#include <delorean/node/AlignedNodeSerDes.hpp>
#include <delorean/node/DirectMappedNodeCache.hpp>
#include <delorean/BasicTypes.hpp>
#include "DirectMappedNodeCacheTest.hpp"

using namespace delo;

CPPUNIT_TEST_SUITE_REGISTRATION(DirectMappedNodeCacheTest);

void DirectMappedNodeCacheTest::testConstructorAndAttributes()
{
    // try building with a size that's not a power of two
    CPPUNIT_ASSERT_THROW(DirectMappedNodeCache cache {205}, std::invalid_argument);

    // build a valid cache
    DirectMappedNodeCache cache {256};

    // verify size
    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(256), cache.getSize());
}

void DirectMappedNodeCacheTest::testGetNode()
{
    // build a few nodes
    std::unique_ptr<AlignedNodeSerDes> serdes {new AlignedNodeSerDes};
    Node::SP node1 {new Node {1024, 4, 5, 0, 0, serdes.get()}};
    Node::SP node2 {new Node {1024, 4, 7, 0, 0, serdes.get()}};
    Node::SP node3 {new Node {1024, 4, 2, 0, 0, serdes.get()}};
    Node::SP node4 {new Node {1024, 4, 3, 0, 0, serdes.get()}};
    Node::SP node5 {new Node {1024, 4, 9, 0, 0, serdes.get()}};
    Node::SP node6 {new Node {1024, 4, 1, 0, 0, serdes.get()}};
    Node::SP node7 {new Node {1024, 4, 6, 0, 0, serdes.get()}};
    std::map<node_seq_t, Node::SP> nodes = {
        {node1->getSeqNumber(), node1},
        {node2->getSeqNumber(), node2},
        {node3->getSeqNumber(), node3},
        {node4->getSeqNumber(), node4},
        {node5->getSeqNumber(), node5},
        {node6->getSeqNumber(), node6},
        {node7->getSeqNumber(), node7}
    };

    // build cache
    std::unique_ptr<DirectMappedNodeCache> cache {
        new DirectMappedNodeCache {4}
    };
    cache->setGetNodeFromOwnerCb([&] (node_seq_t seqNumber) -> Node::SP {
        auto it = nodes.find(seqNumber);

        if (it == nodes.end()) {
            return nullptr;
        }

        return (*it).second;
    });

    // all the nodes should not be cached
    CPPUNIT_ASSERT(!cache->nodeIsCached(5));
    CPPUNIT_ASSERT(!cache->nodeIsCached(7));
    CPPUNIT_ASSERT(!cache->nodeIsCached(2));
    CPPUNIT_ASSERT(!cache->nodeIsCached(3));
    CPPUNIT_ASSERT(!cache->nodeIsCached(9));
    CPPUNIT_ASSERT(!cache->nodeIsCached(1));
    CPPUNIT_ASSERT(!cache->nodeIsCached(6));

    // get some nodes which should exist
    Node::SP node;
    node = cache->getNode(3);
    CPPUNIT_ASSERT_EQUAL(node4, node);
    CPPUNIT_ASSERT(cache->nodeIsCached(3));
    node = cache->getNode(5);
    CPPUNIT_ASSERT_EQUAL(node1, node);
    CPPUNIT_ASSERT(cache->nodeIsCached(3));
    CPPUNIT_ASSERT(cache->nodeIsCached(5));
    node = cache->getNode(9);
    CPPUNIT_ASSERT_EQUAL(node5, node);
    CPPUNIT_ASSERT(cache->nodeIsCached(3));
    CPPUNIT_ASSERT(cache->nodeIsCached(9));
    CPPUNIT_ASSERT(!cache->nodeIsCached(5));

    // get a non existing node
    node = cache->getNode(4);
    CPPUNIT_ASSERT(!node);
    CPPUNIT_ASSERT(!cache->nodeIsCached(4));
}
