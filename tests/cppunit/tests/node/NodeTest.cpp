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
#include <cstring>

#include <delorean/node/AbstractNodeSerDes.hpp>
#include <delorean/node/AlignedNodeSerDes.hpp>
#include <delorean/interval/AbstractInterval.hpp>
#include <delorean/interval/IntInterval.hpp>
#include <delorean/interval/IntervalJar.hpp>
#include <delorean/ex/TimestampOutOfRange.hpp>
#include <delorean/ex/NodeFull.hpp>
#include <delorean/ex/IndexOutOfRange.hpp>
#include <delorean/BasicTypes.hpp>
#include "NodeTest.hpp"

CPPUNIT_TEST_SUITE_REGISTRATION(NodeTest);

class MyNodeSerDes :
    public AbstractNodeSerDes
{
protected:
    void serializeNodeImpl(const Node& node, std::uint8_t* headPtr) const
    {
        // we don't care for this test suite
    }

    std::size_t getHeaderSizeImpl(const Node& node) const
    {
        return 12;
    }

    std::size_t getChildNodePointerSizeImpl(const ChildNodePointer& cnp) const
    {
        return 4;
    }

    std::size_t getIntervalSizeImpl(const AbstractInterval& interval) const
    {
        return 4;
    }

    Node::UP deserializeNodeImpl(const std::uint8_t* headPtr,
                                 std::size_t size,
                                 std::size_t maxChildren) const
    {
        // we don't care for this test suite
        return nullptr;
    }
};

void NodeTest::testConstructorAndAttributes()
{
    // build node
    std::unique_ptr<MyNodeSerDes> serdes {new MyNodeSerDes()};
    Node::UP node {new Node {
        1024,
        16,
        57,
        Node::ROOT_PARENT_SEQ_NUMBER(),
        1534,
        serdes.get()
    }};

    // verify all expected attributes
    CPPUNIT_ASSERT_EQUAL(node->getSize(), static_cast<std::size_t>(1024));
    CPPUNIT_ASSERT_EQUAL(node->getMaxChildren(), static_cast<std::size_t>(16));
    CPPUNIT_ASSERT_EQUAL(node->getSeqNumber(), static_cast<node_seq_t>(57));
    CPPUNIT_ASSERT_EQUAL(node->getParentSeqNumber(), Node::ROOT_PARENT_SEQ_NUMBER());
    CPPUNIT_ASSERT_EQUAL(node->getBegin(), static_cast<timestamp_t>(1534));

    // end should be the same as begin now
    CPPUNIT_ASSERT_EQUAL(node->getEnd(), static_cast<timestamp_t>(1534));

    // not closed
    CPPUNIT_ASSERT(!node->isClosed());

    // not extended
    CPPUNIT_ASSERT(!node->isExtended());

    // set a different parent number
    node->setParentSeqNumber(4);
    CPPUNIT_ASSERT_EQUAL(node->getParentSeqNumber(), static_cast<node_seq_t>(4));

    // no intervals yet
    CPPUNIT_ASSERT_EQUAL(node->getIntervalCount(), static_cast<std::size_t>(0));

    // no children yet
    CPPUNIT_ASSERT_EQUAL(node->getChildrenCount(), static_cast<std::size_t>(0));

    // not full of children either
    CPPUNIT_ASSERT(!node->isFull());

    // closing with an end timestamp smaller than the current end timestamp should throw
    try {
        node->close(1533);
        CPPUNIT_FAIL("Closing node with end timestamp smaller than current");
    } catch (const TimestampOutOfRange& ex) {
        CPPUNIT_ASSERT_EQUAL(ex.getTs(), static_cast<timestamp_t>(1533));
    }

    // this should work, change the end timestamp and mark it done
    node->close(1608);
    CPPUNIT_ASSERT_EQUAL(node->getEnd(), static_cast<timestamp_t>(1608));
    CPPUNIT_ASSERT(node->isClosed());
}

void NodeTest::testAddInterval()
{
    // build node
    std::unique_ptr<MyNodeSerDes> serdes {new MyNodeSerDes()};
    Node::UP node {new Node {
        1024,
        16,
        57,
        Node::ROOT_PARENT_SEQ_NUMBER(),
        1534,
        serdes.get()
    }};

    // create 3 intervals
    IntInterval::SP interval1 {new IntInterval(1602, 1999, 1)};
    IntInterval::SP interval2 {new IntInterval(2000, 3000, 1)};

    // add intervals
    node->addInterval(interval1);
    node->addInterval(interval2);

    // there are 2 intervals now
    CPPUNIT_ASSERT_EQUAL(node->getIntervalCount(), static_cast<std::size_t>(2));

    // make sure they are the same (pointers)
    const auto& intervals = node->getIntervals();
    CPPUNIT_ASSERT(intervals.at(0) == interval1);
    CPPUNIT_ASSERT(intervals.at(1) == interval2);
}

void NodeTest::testIntervalFits()
{
    // let's build a very small node (current size is header: 12 bytes)
    std::unique_ptr<MyNodeSerDes> serdes {new MyNodeSerDes()};
    Node::UP node {new Node {
        32,
        16,
        57,
        Node::ROOT_PARENT_SEQ_NUMBER(),
        1534,
        serdes.get()
    }};

    // add two children: increases size by 8 bytes to 20 bytes
    node->addChild(1600, 0);
    node->addChild(1600, 0);

    // (12 bytes remaining / 4 bytes/interval) == 3 intervals should fit now
    IntInterval::SP interval {new IntInterval(1602, 2000, 1)};

    // first fits
    CPPUNIT_ASSERT(node->intervalFits(*interval));
    node->addInterval(interval);

    // second fits
    CPPUNIT_ASSERT(node->intervalFits(*interval));
    node->addInterval(interval);

    // third fits
    CPPUNIT_ASSERT(node->intervalFits(*interval));
    node->addInterval(interval);

    // fourth doesn't fit
    CPPUNIT_ASSERT(!node->intervalFits(*interval));
}

void NodeTest::testFindOne()
{
    // build node
    std::unique_ptr<MyNodeSerDes> serdes {new MyNodeSerDes()};
    Node::UP node {new Node {
        1024,
        16,
        57,
        Node::ROOT_PARENT_SEQ_NUMBER(),
        1534,
        serdes.get()
    }};

    // create a few intervals
    IntInterval::SP interval1 {new IntInterval(1534, 1608, 1)};
    IntInterval::SP interval2 {new IntInterval(1602, 1777, 2)};
    IntInterval::SP interval3 {new IntInterval(1540, 1867, 3)};
    IntInterval::SP interval4 {new IntInterval(1540, 1914, 4)};
    IntInterval::SP interval5 {new IntInterval(1918, 1939, 5)};
    IntInterval::SP interval6 {new IntInterval(1939, 1945, 6)};

    // add them
    node->addInterval(interval1);
    node->addInterval(interval2);
    node->addInterval(interval3);
    node->addInterval(interval4);
    node->addInterval(interval5);
    node->addInterval(interval6);

    // working searches
    AbstractInterval::SP result {};

    result = node->findOne(1534, 1);
    CPPUNIT_ASSERT(result == interval1);

    result = node->findOne(1534, 2);
    CPPUNIT_ASSERT(result == nullptr);

    result = node->findOne(1605, 1);
    CPPUNIT_ASSERT(result == interval1);

    result = node->findOne(1605, 2);
    CPPUNIT_ASSERT(result == interval2);

    result = node->findOne(1605, 5);
    CPPUNIT_ASSERT(result == nullptr);

    result = node->findOne(1915, 4);
    CPPUNIT_ASSERT(result == nullptr);

    result = node->findOne(1915, 5);
    CPPUNIT_ASSERT(result == nullptr);

    result = node->findOne(1939, 5);
    CPPUNIT_ASSERT(result == interval5);

    result = node->findOne(1939, 6);
    CPPUNIT_ASSERT(result == interval6);
}

void NodeTest::testFindAll()
{
    // build node
    std::unique_ptr<MyNodeSerDes> serdes {new MyNodeSerDes()};
    Node::UP node {new Node {
        1024,
        16,
        57,
        Node::ROOT_PARENT_SEQ_NUMBER(),
        1534,
        serdes.get()
    }};

    // create a few intervals
    IntInterval::SP interval1 {new IntInterval(1534, 1608, 1)};
    IntInterval::SP interval2 {new IntInterval(1602, 1777, 2)};
    IntInterval::SP interval3 {new IntInterval(1540, 1777, 3)};
    IntInterval::SP interval4 {new IntInterval(1541, 1914, 4)};
    IntInterval::SP interval5 {new IntInterval(1918, 1939, 5)};
    IntInterval::SP interval6 {new IntInterval(1939, 1945, 6)};

    // add them
    node->addInterval(interval1);
    node->addInterval(interval2);
    node->addInterval(interval3);
    node->addInterval(interval4);
    node->addInterval(interval5);
    node->addInterval(interval6);

    // initiate an interval jar
    std::unique_ptr<IntervalJar> jar {new IntervalJar()};

    // working searches
    CPPUNIT_ASSERT(node->findAll(1534, *jar));
    CPPUNIT_ASSERT(jar->size() == 1);
    CPPUNIT_ASSERT(jar->at(1) == interval1);
    jar->clear();

    CPPUNIT_ASSERT(node->findAll(1545, *jar));
    CPPUNIT_ASSERT(jar->size() == 3);
    CPPUNIT_ASSERT(jar->at(1) == interval1);
    CPPUNIT_ASSERT(jar->at(3) == interval3);
    CPPUNIT_ASSERT(jar->at(4) == interval4);
    jar->clear();

    CPPUNIT_ASSERT(node->findAll(1777, *jar));
    CPPUNIT_ASSERT(jar->size() == 3);
    CPPUNIT_ASSERT(jar->at(2) == interval2);
    CPPUNIT_ASSERT(jar->at(3) == interval3);
    CPPUNIT_ASSERT(jar->at(4) == interval4);
    jar->clear();

    CPPUNIT_ASSERT(node->findAll(1939, *jar));
    CPPUNIT_ASSERT(jar->size() == 2);
    CPPUNIT_ASSERT(jar->at(5) == interval5);
    CPPUNIT_ASSERT(jar->at(6) == interval6);
    jar->clear();

    CPPUNIT_ASSERT(!node->findAll(1915, *jar));
    CPPUNIT_ASSERT(jar->empty());

    CPPUNIT_ASSERT(!node->findAll(1917, *jar));
    CPPUNIT_ASSERT(jar->empty());
}

void NodeTest::testChildren()
{
    // build node: max 4 children
    std::unique_ptr<MyNodeSerDes> serdes {new MyNodeSerDes()};
    Node::UP node {new Node {
        1024,
        4,
        57,
        Node::ROOT_PARENT_SEQ_NUMBER(),
        1534,
        serdes.get()
    }};

    // add 4 children
    node->addChild(1600, 10);
    node->addChild(1700, 11);
    node->addChild(1800, 12);
    node->addChild(1900, 13);
    CPPUNIT_ASSERT_EQUAL(node->getChildrenCount(), static_cast<std::size_t>(4));

    // make sure we can't one more child
    try {
        node->addChild(2000, 14);
        CPPUNIT_FAIL("Adding a child when the node is full");
    } catch (const NodeFull& ex) {
    }
    CPPUNIT_ASSERT(node->isFull());

    // get child sequence number at index
    CPPUNIT_ASSERT_EQUAL(node->getChildSeqAtIndex(0), static_cast<node_seq_t>(10));
    CPPUNIT_ASSERT_EQUAL(node->getChildSeqAtIndex(1), static_cast<node_seq_t>(11));
    CPPUNIT_ASSERT_EQUAL(node->getChildSeqAtIndex(2), static_cast<node_seq_t>(12));
    CPPUNIT_ASSERT_EQUAL(node->getChildSeqAtIndex(3), static_cast<node_seq_t>(13));
    try {
        node->getChildSeqAtIndex(4);
        CPPUNIT_FAIL("Getting a child at too big index");
    } catch (const IndexOutOfRange& ex) {
        CPPUNIT_ASSERT_EQUAL(ex.getSize(), static_cast<std::size_t>(4));
        CPPUNIT_ASSERT_EQUAL(ex.getIndex(), static_cast<std::size_t>(4));
    }

    // get child begin timestamp at index
    CPPUNIT_ASSERT_EQUAL(node->getChildBeginAtIndex(0), static_cast<timestamp_t>(1600));
    CPPUNIT_ASSERT_EQUAL(node->getChildBeginAtIndex(1), static_cast<timestamp_t>(1700));
    CPPUNIT_ASSERT_EQUAL(node->getChildBeginAtIndex(2), static_cast<timestamp_t>(1800));
    CPPUNIT_ASSERT_EQUAL(node->getChildBeginAtIndex(3), static_cast<timestamp_t>(1900));
    try {
        node->getChildBeginAtIndex(4);
        CPPUNIT_FAIL("Getting a child at too big index");
    } catch (const IndexOutOfRange& ex) {
        CPPUNIT_ASSERT_EQUAL(ex.getSize(), static_cast<std::size_t>(4));
        CPPUNIT_ASSERT_EQUAL(ex.getIndex(), static_cast<std::size_t>(4));
    }

    // get child sequence number at timestamp
    CPPUNIT_ASSERT_EQUAL(node->getChildSeqAtTs(1534), static_cast<node_seq_t>(57));
    CPPUNIT_ASSERT_EQUAL(node->getChildSeqAtTs(1575), static_cast<node_seq_t>(57));
    CPPUNIT_ASSERT_EQUAL(node->getChildSeqAtTs(1599), static_cast<node_seq_t>(57));
    CPPUNIT_ASSERT_EQUAL(node->getChildSeqAtTs(1600), static_cast<node_seq_t>(10));
    CPPUNIT_ASSERT_EQUAL(node->getChildSeqAtTs(1699), static_cast<node_seq_t>(10));
    CPPUNIT_ASSERT_EQUAL(node->getChildSeqAtTs(1700), static_cast<node_seq_t>(11));
    CPPUNIT_ASSERT_EQUAL(node->getChildSeqAtTs(1799), static_cast<node_seq_t>(11));
    CPPUNIT_ASSERT_EQUAL(node->getChildSeqAtTs(1800), static_cast<node_seq_t>(12));
    CPPUNIT_ASSERT_EQUAL(node->getChildSeqAtTs(1899), static_cast<node_seq_t>(12));
    CPPUNIT_ASSERT_EQUAL(node->getChildSeqAtTs(1900), static_cast<node_seq_t>(13));
    CPPUNIT_ASSERT_EQUAL(node->getChildSeqAtTs(1999), static_cast<node_seq_t>(13));
    CPPUNIT_ASSERT_EQUAL(node->getChildSeqAtTs(2000), static_cast<node_seq_t>(13));
}
