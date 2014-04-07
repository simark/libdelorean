/**
 * Copyright (c) 2014 Philippe Proulx <eepp.ca>
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

#include <delorean/interval/IntInterval.hpp>
#include <delorean/interval/StandardIntervalTypes.hpp>
#include <delorean/ex/InvalidIntervalArguments.hpp>
#include <delorean/BasicTypes.hpp>
#include "IntIntervalTest.hpp"

CPPUNIT_TEST_SUITE_REGISTRATION(IntIntervalTest);

void IntIntervalTest::testConstructorAndAttributes()
{
    timestamp_t begin = 100;
    timestamp_t end = 40;
    interval_id_t id = 4;

    // wrong interval parameters
    try {
        IntInterval::UP interval {new IntInterval(begin, end, id)};
        CPPUNIT_FAIL("Building interval with wrong parameters");
    } catch (const InvalidIntervalArguments& ex) {
        CPPUNIT_ASSERT_EQUAL(ex.getBegin(), begin);
        CPPUNIT_ASSERT_EQUAL(ex.getEnd(), end);
    }

    // begin == end is okay
    begin = 50;
    end = 50;
    try {
        IntInterval::UP interval {new IntInterval(begin, end, id)};
    } catch (const InvalidIntervalArguments& ex) {
        CPPUNIT_FAIL("Interval constructor does not accept equal begin and end times");
    }

    // attributes
    begin = 23;
    end = 67;
    id = 5;
    IntInterval::UP interval {new IntInterval(begin, end, id)};
    CPPUNIT_ASSERT_EQUAL(interval->getBegin(), begin);
    CPPUNIT_ASSERT_EQUAL(interval->getEnd(), end);
    CPPUNIT_ASSERT_EQUAL(interval->getId(), id);
    CPPUNIT_ASSERT_EQUAL(interval->getType(),
        static_cast<interval_type_t>(StandardIntervalTypes::INT32));

    // value
    int value = 31987238;
    interval->setValue(value);
    CPPUNIT_ASSERT_EQUAL(interval->getValue(), value);
}

void IntIntervalTest::testIntersection()
{
    // build valid interval
    IntInterval::UP interval {new IntInterval(50, 150, 1)};

    // intersection should be inclusive
    CPPUNIT_ASSERT(!interval->intersects(49));
    CPPUNIT_ASSERT(interval->intersects(50));
    CPPUNIT_ASSERT(interval->intersects(51));
    CPPUNIT_ASSERT(interval->intersects(149));
    CPPUNIT_ASSERT(interval->intersects(150));
    CPPUNIT_ASSERT(!interval->intersects(151));
}

void IntIntervalTest::testOperators()
{
    // operators (<, <=, ==, >=, >) compare end time only

    // prepare some intervals
    IntInterval::UP interval1 {new IntInterval(1534, 1867, 1)};
    IntInterval::UP interval2 {new IntInterval(1988, 2014, 2)};
    IntInterval::UP interval3 {new IntInterval(2001, 2014, 3)};

    // <
    CPPUNIT_ASSERT(*interval1 < *interval2);
    CPPUNIT_ASSERT(!(*interval2 < *interval1));

    // <=
    CPPUNIT_ASSERT(*interval1 <= *interval2);
    CPPUNIT_ASSERT(!(*interval2 <= *interval1));
    CPPUNIT_ASSERT(*interval2 <= *interval3);
    CPPUNIT_ASSERT(*interval3 <= *interval2);

    // ==, !=
    CPPUNIT_ASSERT(*interval2 == *interval3);
    CPPUNIT_ASSERT(!(*interval1 == *interval2));
    CPPUNIT_ASSERT(!(*interval2 != *interval3));
    CPPUNIT_ASSERT(*interval1 != *interval2);

    // >=
    CPPUNIT_ASSERT(*interval2 >= *interval1);
    CPPUNIT_ASSERT(!(*interval1 >= *interval2));
    CPPUNIT_ASSERT(*interval2 >= *interval3);
    CPPUNIT_ASSERT(*interval3 >= *interval2);

    // >
    CPPUNIT_ASSERT(*interval2 > *interval1);
    CPPUNIT_ASSERT(!(*interval1 > *interval2));
}

void IntIntervalTest::testSize()
{
    IntInterval::UP interval {new IntInterval(1534, 1867, 1)};

    // no variable data
    CPPUNIT_ASSERT_EQUAL(interval->getVariableSize(), static_cast<std::size_t>(0));

    /* Both 24 bytes because this interval type doesn't have variable data:
     *
     *     begin:   8
     *     end:     8
     *     type/ID: 4
     *     value:   4
     */
    CPPUNIT_ASSERT_EQUAL(interval->getHeaderSize(), static_cast<std::size_t>(24));
    CPPUNIT_ASSERT_EQUAL(interval->getSize(), static_cast<std::size_t>(24));
}

void IntIntervalTest::testValueSerialization()
{
    // allocate and initialize a buffer
    std::unique_ptr<std::uint8_t[]> bufUp {new std::uint8_t [1024]};
    auto buf = bufUp.get();
    for (unsigned int x = 0; x < 1024; ++x) {
        buf[x] = x & 0xff;
    }

    // create interval with custom value
    IntInterval::UP interval {new IntInterval(1608, 2008, 5)};
    interval->setValue(0x12abcdef);

    // serialize values
    interval->serializeValues(buf, buf + 1024);

    // check serialized value
    std::int32_t i;
    std::memcpy(&i, buf, 4);
    CPPUNIT_ASSERT_EQUAL(i, static_cast<int32_t>(0x12abcdef));

    // make sure the rest of the buffer wasn't altered (no variable data)
    for (unsigned int x = 4; x < 1024; ++x) {
        CPPUNIT_ASSERT_EQUAL(buf[x], static_cast<uint8_t>(x & 0xff));
    }
}

void IntIntervalTest::testValueDeserialization()
{
    // allocate and initialize a buffer with a value
    std::unique_ptr<std::uint8_t[]> bufUp {new std::uint8_t [1024]};
    auto buf = bufUp.get();
    std::int32_t value = 0x11223344;
    std::memcpy(buf, &value, 4);

    // create interval
    IntInterval::UP interval {new IntInterval(1608, 2008, 5)};

    // deserialize values
    interval->deserializeValues(buf, buf + 1024);

    // check deserialized value
    CPPUNIT_ASSERT_EQUAL(static_cast<int>(value), interval->getValue());
}
