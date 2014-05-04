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
#include <delorean/interval/Int32Interval.hpp>
#include <delorean/interval/IntervalJar.hpp>
#include <delorean/ex/TimestampOutOfRange.hpp>
#include <delorean/ex/NodeFull.hpp>
#include <delorean/ex/IndexOutOfRange.hpp>
#include <delorean/BasicTypes.hpp>
#include "NodeTest.hpp"

using namespace delo;

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
    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(1024), node->getSize());
    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(16), node->getMaxChildren());
    CPPUNIT_ASSERT_EQUAL(static_cast<node_seq_t>(57), node->getSeqNumber());
    CPPUNIT_ASSERT_EQUAL(Node::ROOT_PARENT_SEQ_NUMBER(), node->getParentSeqNumber());
    CPPUNIT_ASSERT_EQUAL(static_cast<timestamp_t>(1534), node->getBegin());

    // end should be the same as begin now
    CPPUNIT_ASSERT_EQUAL(static_cast<timestamp_t>(1534), node->getEnd());

    // not closed
    CPPUNIT_ASSERT(!node->isClosed());

    // not extended
    CPPUNIT_ASSERT(!node->isExtended());

    // set a different parent number
    node->setParentSeqNumber(4);
    CPPUNIT_ASSERT_EQUAL(static_cast<node_seq_t>(4), node->getParentSeqNumber());

    // no intervals yet
    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(0), node->getIntervalCount());

    // no children yet
    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(0), node->getChildrenCount());

    // not full of children either
    CPPUNIT_ASSERT(!node->isFull());

    // closing with an end timestamp smaller than the current end timestamp should throw
    try {
        node->close(1533);
        CPPUNIT_FAIL("Closing node with end timestamp smaller than current");
    } catch (const ex::TimestampOutOfRange& ex) {
        CPPUNIT_ASSERT_EQUAL(static_cast<timestamp_t>(1533), ex.getTs());
    }

    // this should work, change the end timestamp and mark it done
    node->close(1608);
    CPPUNIT_ASSERT_EQUAL(static_cast<timestamp_t>(1608), node->getEnd());
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
    Int32Interval::SP interval1 {new Int32Interval(1602, 1999, 1)};
    Int32Interval::SP interval2 {new Int32Interval(2000, 3000, 1)};

    // add intervals
    node->addInterval(interval1);
    node->addInterval(interval2);

    // there are 2 intervals now
    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(2), node->getIntervalCount());

    // make sure they are the same (pointers)
    const auto& intervals = node->getIntervals();
    CPPUNIT_ASSERT(intervals[0] == interval1);
    CPPUNIT_ASSERT(intervals[1] == interval2);
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
    Int32Interval::SP interval {new Int32Interval(1602, 2000, 1)};

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
    Int32Interval::SP interval1 {new Int32Interval(1534, 1608, 1)};
    Int32Interval::SP interval2 {new Int32Interval(1602, 1777, 2)};
    Int32Interval::SP interval3 {new Int32Interval(1540, 1867, 3)};
    Int32Interval::SP interval4 {new Int32Interval(1540, 1914, 4)};
    Int32Interval::SP interval5 {new Int32Interval(1918, 1939, 5)};
    Int32Interval::SP interval6 {new Int32Interval(1939, 1945, 6)};

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
    CPPUNIT_ASSERT(!result);

    result = node->findOne(1605, 1);
    CPPUNIT_ASSERT(result == interval1);

    result = node->findOne(1605, 2);
    CPPUNIT_ASSERT(result == interval2);

    result = node->findOne(1605, 5);
    CPPUNIT_ASSERT(!result);

    result = node->findOne(1915, 4);
    CPPUNIT_ASSERT(!result);

    result = node->findOne(1915, 5);
    CPPUNIT_ASSERT(!result);

    result = node->findOne(1939, 5);
    CPPUNIT_ASSERT(!result);

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
    Int32Interval::SP interval1 {new Int32Interval(1534, 1608, 1)};
    Int32Interval::SP interval2 {new Int32Interval(1602, 1777, 2)};
    Int32Interval::SP interval3 {new Int32Interval(1540, 1777, 3)};
    Int32Interval::SP interval4 {new Int32Interval(1541, 1914, 4)};
    Int32Interval::SP interval5 {new Int32Interval(1918, 1939, 5)};
    Int32Interval::SP interval6 {new Int32Interval(1939, 1945, 6)};

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
    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(1), jar->size());
    CPPUNIT_ASSERT(jar->at(1) == interval1);
    jar->clear();

    CPPUNIT_ASSERT(node->findAll(1545, *jar));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(3), jar->size());
    CPPUNIT_ASSERT(jar->at(1) == interval1);
    CPPUNIT_ASSERT(jar->at(3) == interval3);
    CPPUNIT_ASSERT(jar->at(4) == interval4);
    jar->clear();

    CPPUNIT_ASSERT(node->findAll(1777, *jar));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(1), jar->size());
    CPPUNIT_ASSERT(jar->at(4) == interval4);
    jar->clear();

    CPPUNIT_ASSERT(node->findAll(1939, *jar));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(1), jar->size());
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
    } catch (const ex::NodeFull& ex) {
    }
    CPPUNIT_ASSERT(node->isFull());

    // get child sequence number at index
    CPPUNIT_ASSERT_EQUAL(static_cast<node_seq_t>(10), node->getChildSeqAtIndex(0));
    CPPUNIT_ASSERT_EQUAL(static_cast<node_seq_t>(11), node->getChildSeqAtIndex(1));
    CPPUNIT_ASSERT_EQUAL(static_cast<node_seq_t>(12), node->getChildSeqAtIndex(2));
    CPPUNIT_ASSERT_EQUAL(static_cast<node_seq_t>(13), node->getChildSeqAtIndex(3));
    try {
        node->getChildSeqAtIndex(4);
        CPPUNIT_FAIL("Getting a child at too big index");
    } catch (const ex::IndexOutOfRange& ex) {
        CPPUNIT_ASSERT_EQUAL(ex.getSize(), static_cast<std::size_t>(4));
        CPPUNIT_ASSERT_EQUAL(ex.getIndex(), static_cast<std::size_t>(4));
    }

    // get child begin timestamp at index
    CPPUNIT_ASSERT_EQUAL(static_cast<timestamp_t>(1600), node->getChildBeginAtIndex(0));
    CPPUNIT_ASSERT_EQUAL(static_cast<timestamp_t>(1700), node->getChildBeginAtIndex(1));
    CPPUNIT_ASSERT_EQUAL(static_cast<timestamp_t>(1800), node->getChildBeginAtIndex(2));
    CPPUNIT_ASSERT_EQUAL(static_cast<timestamp_t>(1900), node->getChildBeginAtIndex(3));
    try {
        node->getChildBeginAtIndex(4);
        CPPUNIT_FAIL("Getting a child at too big index");
    } catch (const ex::IndexOutOfRange& ex) {
        CPPUNIT_ASSERT_EQUAL(ex.getSize(), static_cast<std::size_t>(4));
        CPPUNIT_ASSERT_EQUAL(ex.getIndex(), static_cast<std::size_t>(4));
    }

    // get child sequence number at timestamp
    CPPUNIT_ASSERT_EQUAL(static_cast<node_seq_t>(57), node->getChildSeqAtTs(1534));
    CPPUNIT_ASSERT_EQUAL(static_cast<node_seq_t>(57), node->getChildSeqAtTs(1575));
    CPPUNIT_ASSERT_EQUAL(static_cast<node_seq_t>(57), node->getChildSeqAtTs(1599));
    CPPUNIT_ASSERT_EQUAL(static_cast<node_seq_t>(10), node->getChildSeqAtTs(1600));
    CPPUNIT_ASSERT_EQUAL(static_cast<node_seq_t>(10), node->getChildSeqAtTs(1699));
    CPPUNIT_ASSERT_EQUAL(static_cast<node_seq_t>(11), node->getChildSeqAtTs(1700));
    CPPUNIT_ASSERT_EQUAL(static_cast<node_seq_t>(11), node->getChildSeqAtTs(1799));
    CPPUNIT_ASSERT_EQUAL(static_cast<node_seq_t>(12), node->getChildSeqAtTs(1800));
    CPPUNIT_ASSERT_EQUAL(static_cast<node_seq_t>(12), node->getChildSeqAtTs(1899));
    CPPUNIT_ASSERT_EQUAL(static_cast<node_seq_t>(13), node->getChildSeqAtTs(1900));
    CPPUNIT_ASSERT_EQUAL(static_cast<node_seq_t>(13), node->getChildSeqAtTs(1999));
    CPPUNIT_ASSERT_EQUAL(static_cast<node_seq_t>(13), node->getChildSeqAtTs(2000));
}
