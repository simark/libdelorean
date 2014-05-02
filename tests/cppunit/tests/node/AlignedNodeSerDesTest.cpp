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

#include <delorean/node/AlignedNodeSerDes.hpp>
#include <delorean/interval/StringInterval.hpp>
#include <delorean/interval/IntervalJar.hpp>
#include <delorean/BasicTypes.hpp>
#include "AlignedNodeSerDesTest.hpp"

CPPUNIT_TEST_SUITE_REGISTRATION(AlignedNodeSerDesTest);

void AlignedNodeSerDesTest::testSerializeDeserialize()
{
    // create intervals (important time periods in Africa)
    StringInterval::SP interval1 {new StringInterval {-3000, -2000, 1}};
    StringInterval::SP interval2 {new StringInterval {-1999, -1300, 2}};
    StringInterval::SP interval3 {new StringInterval {-1299, -1070, 3}};
    StringInterval::SP interval4 {new StringInterval {-305, -30, 4}};
    StringInterval::SP interval5 {new StringInterval {-29, 390, 5}};
    StringInterval::SP interval6 {new StringInterval {391, 900, 6}};
    StringInterval::SP interval7 {new StringInterval {909, 1171, 7}};
    StringInterval::SP interval8 {new StringInterval {1172, 1250, 8}};
    StringInterval::SP interval9 {new StringInterval {1251, 1517, 9}};
    StringInterval::SP interval10 {new StringInterval {1518, 1867, 10}};
    StringInterval::SP interval11 {new StringInterval {1868, 1914, 11}};
    interval1->setValue("Old Kingdom");
    interval2->setValue("Middle Kingdom");
    interval3->setValue("New Kingdom");
    interval4->setValue("Ptolemaic Kingdom");
    interval5->setValue("Aegyptus");
    interval6->setValue("Coptic period");
    interval7->setValue("Fatimid Caliphate");
    interval8->setValue("Ayyubid Dynasty");
    interval9->setValue("Mamluk Sultanate");
    interval10->setValue("Ottoman Eyalet");
    interval11->setValue("Khedivate");

    // create aligned serializer/deserializer
    std::unique_ptr<AlignedNodeSerDes> serdes {new AlignedNodeSerDes};

    // create node
    Node::UP node {new Node {
        2048,
        4,
        5,
        Node::ROOT_PARENT_SEQ_NUMBER(),
        -3000,
        serdes.get()
    }};

    // add intervals to node
    std::vector<AbstractInterval::SP> jar;
    jar.push_back(interval1);
    jar.push_back(interval2);
    jar.push_back(interval3);
    jar.push_back(interval4);
    jar.push_back(interval5);
    jar.push_back(interval6);
    jar.push_back(interval7);
    jar.push_back(interval8);
    jar.push_back(interval9);
    jar.push_back(interval10);
    jar.push_back(interval11);
    for (const auto& interval : jar) {
        node->addInterval(interval);
    }
    node->close(1939);

    // add some children
    node->addChild(-3000, 8);
    node->addChild(-2700, 17);
    node->addChild(152, 3);

    // allocate buffer for serializing
    std::unique_ptr<std::uint8_t[]> bufUp {new std::uint8_t [2048]};
    auto buf = bufUp.get();

    // serialize node
    serdes->serializeNode(*node, buf);

    // deserialize node
    auto deserNode = serdes->deserializeNode(buf, 2048, 4);

    // verify node attributes
    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(2048), deserNode->getSize());
    CPPUNIT_ASSERT_EQUAL(static_cast<timestamp_t>(-3000), deserNode->getBegin());
    CPPUNIT_ASSERT_EQUAL(static_cast<timestamp_t>(1939), deserNode->getEnd());
    CPPUNIT_ASSERT(!deserNode->isFull());
    CPPUNIT_ASSERT_EQUAL(static_cast<node_seq_t>(5), deserNode->getSeqNumber());
    CPPUNIT_ASSERT_EQUAL(Node::ROOT_PARENT_SEQ_NUMBER(), deserNode->getParentSeqNumber());
    CPPUNIT_ASSERT(deserNode->isClosed());
    CPPUNIT_ASSERT(!deserNode->isExtended());
    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(4), deserNode->getMaxChildren());

    // verify children
    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(3), deserNode->getChildrenCount());
    auto& children = deserNode->getChildren();
    CPPUNIT_ASSERT_EQUAL(static_cast<timestamp_t>(-3000), children[0].getBegin());
    CPPUNIT_ASSERT_EQUAL(static_cast<node_seq_t>(8), children[0].getSeqNumber());
    CPPUNIT_ASSERT_EQUAL(static_cast<timestamp_t>(-2700), children[1].getBegin());
    CPPUNIT_ASSERT_EQUAL(static_cast<node_seq_t>(17), children[1].getSeqNumber());
    CPPUNIT_ASSERT_EQUAL(static_cast<timestamp_t>(152), children[2].getBegin());
    CPPUNIT_ASSERT_EQUAL(static_cast<node_seq_t>(3), children[2].getSeqNumber());

    // verify intervals
    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(11), deserNode->getIntervalCount());
    auto& intervals = deserNode->getIntervals();
    auto origIt = jar.begin();
    for (const auto& deserInterval : intervals) {
        auto& deserStrInterval = static_cast<const StringInterval&>(*deserInterval);
        auto& origInterval = static_cast<StringInterval&>(**origIt);
        origIt++;

        // compare intervals
        CPPUNIT_ASSERT_EQUAL(origInterval.getType(), deserStrInterval.getType());
        CPPUNIT_ASSERT_EQUAL(origInterval.getBegin(), deserStrInterval.getBegin());
        CPPUNIT_ASSERT_EQUAL(origInterval.getEnd(), deserStrInterval.getEnd());
        CPPUNIT_ASSERT_EQUAL(origInterval.getKey(), deserStrInterval.getKey());
        CPPUNIT_ASSERT_EQUAL(origInterval.getValue(), deserStrInterval.getValue());
    }
}
