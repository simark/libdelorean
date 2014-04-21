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

#include <delorean/interval/IntInterval.hpp>
#include <delorean/interval/StandardIntervalType.hpp>
#include <delorean/ex/InvalidIntervalArguments.hpp>
#include <delorean/BasicTypes.hpp>
#include "IntIntervalTest.hpp"

CPPUNIT_TEST_SUITE_REGISTRATION(IntIntervalTest);

void IntIntervalTest::testConstructorAndAttributes()
{
    timestamp_t begin = 100;
    timestamp_t end = 40;
    interval_cat_id_t catId = 4;

    // wrong interval parameters
    try {
        IntInterval::UP interval {new IntInterval(begin, end, catId)};
        CPPUNIT_FAIL("Building interval with wrong parameters");
    } catch (const InvalidIntervalArguments& ex) {
        CPPUNIT_ASSERT_EQUAL(ex.getBegin(), begin);
        CPPUNIT_ASSERT_EQUAL(ex.getEnd(), end);
    }

    // begin == end is okay
    begin = 50;
    end = 50;
    try {
        IntInterval::UP interval {new IntInterval(begin, end, catId)};
    } catch (const InvalidIntervalArguments& ex) {
        CPPUNIT_FAIL("Interval constructor does not accept equal begin and end times");
    }

    // attributes
    begin = 23;
    end = 67;
    catId = 5;
    IntInterval::UP interval {new IntInterval(begin, end, catId)};
    CPPUNIT_ASSERT_EQUAL(interval->getBegin(), begin);
    CPPUNIT_ASSERT_EQUAL(interval->getEnd(), end);
    CPPUNIT_ASSERT_EQUAL(interval->getCatId(), catId);
    CPPUNIT_ASSERT_EQUAL(interval->getType(),
        static_cast<interval_type_t>(StandardIntervalType::INT32));

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

void IntIntervalTest::testFixedValue()
{
    IntInterval::UP interval {new IntInterval(1939, 1945, 666)};

    // set fixed, get native
    int i = -1001;
    interval->setFixedValue(static_cast<uint32_t>(i));
    CPPUNIT_ASSERT_EQUAL(interval->getValue(), i);

    // set native, get fixed
    uint32_t u = 0xfffffffe;
    interval->setValue(static_cast<int>(u));
    CPPUNIT_ASSERT_EQUAL(interval->getFixedValue(), u);
}

void IntIntervalTest::testVariableDataSize()
{
    IntInterval::UP interval {new IntInterval(1534, 1867, 1)};

    // no variable data
    CPPUNIT_ASSERT_EQUAL(interval->getVariableDataSize(), static_cast<std::size_t>(0));
}
