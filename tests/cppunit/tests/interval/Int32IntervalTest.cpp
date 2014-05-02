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

#include <delorean/interval/Int32Interval.hpp>
#include <delorean/interval/StandardIntervalType.hpp>
#include <delorean/ex/InvalidIntervalArguments.hpp>
#include <delorean/BasicTypes.hpp>
#include "Int32IntervalTest.hpp"

CPPUNIT_TEST_SUITE_REGISTRATION(Int32IntervalTest);

void Int32IntervalTest::testConstructorAndAttributes()
{
    timestamp_t begin = 100;
    timestamp_t end = 40;
    interval_key_t key = 4;

    // wrong interval parameters
    try {
        Int32Interval::UP interval {new Int32Interval(begin, end, key)};
        CPPUNIT_FAIL("Building interval with wrong parameters");
    } catch (const InvalidIntervalArguments& ex) {
        CPPUNIT_ASSERT_EQUAL(begin, ex.getBegin());
        CPPUNIT_ASSERT_EQUAL(end, ex.getEnd());
    }

    // begin == end is okay
    begin = 50;
    end = 50;
    try {
        Int32Interval::UP interval {new Int32Interval(begin, end, key)};
    } catch (const InvalidIntervalArguments& ex) {
        CPPUNIT_FAIL("Interval constructor does not accept equal begin and end times");
    }

    // attributes
    begin = 23;
    end = 67;
    key = 5;
    Int32Interval::UP interval {new Int32Interval(begin, end, key)};
    CPPUNIT_ASSERT_EQUAL(begin, interval->getBegin());
    CPPUNIT_ASSERT_EQUAL(end, interval->getEnd());
    CPPUNIT_ASSERT_EQUAL(key, interval->getKey());
    CPPUNIT_ASSERT_EQUAL(static_cast<interval_type_t>(StandardIntervalType::INT32),
                         interval->getType());

    // value
    int value = 31987238;
    interval->setValue(value);
    CPPUNIT_ASSERT_EQUAL(value, interval->getValue());
}

void Int32IntervalTest::testIntersection()
{
    // build valid interval
    Int32Interval::UP interval {new Int32Interval(50, 150, 1)};

    // intersection should include begin, but exclude end
    CPPUNIT_ASSERT(!interval->intersects(49));
    CPPUNIT_ASSERT(interval->intersects(50));
    CPPUNIT_ASSERT(interval->intersects(51));
    CPPUNIT_ASSERT(interval->intersects(149));
    CPPUNIT_ASSERT(!interval->intersects(150));
    CPPUNIT_ASSERT(!interval->intersects(151));
}

void Int32IntervalTest::testOperators()
{
    // operators (<, <=, ==, >=, >) compare end time only

    // prepare some intervals
    Int32Interval::UP interval1 {new Int32Interval(1534, 1867, 1)};
    Int32Interval::UP interval2 {new Int32Interval(1988, 2014, 2)};
    Int32Interval::UP interval3 {new Int32Interval(2001, 2014, 3)};

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

void Int32IntervalTest::testFixedValue()
{
    Int32Interval::UP interval {new Int32Interval(1939, 1945, 666)};

    // set fixed, get native
    int i = -1001;
    interval->setFixedValue(static_cast<uint32_t>(i));
    CPPUNIT_ASSERT_EQUAL(i, interval->getValue());

    // set native, get fixed
    uint32_t u = 0xfffffffe;
    interval->setValue(static_cast<int>(u));
    CPPUNIT_ASSERT_EQUAL(u, interval->getFixedValue());
}

void Int32IntervalTest::testVariableDataSize()
{
    Int32Interval::UP interval {new Int32Interval(1534, 1867, 1)};

    // no variable data
    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(0), interval->getVariableDataSize());
}
