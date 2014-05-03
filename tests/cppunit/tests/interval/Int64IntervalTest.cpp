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

#include <delorean/interval/Int64Interval.hpp>
#include <delorean/interval/StandardIntervalType.hpp>
#include <delorean/ex/InvalidIntervalArguments.hpp>
#include <delorean/BasicTypes.hpp>
#include "Int64IntervalTest.hpp"

using namespace delo;

CPPUNIT_TEST_SUITE_REGISTRATION(Int64IntervalTest);

void Int64IntervalTest::testValue()
{
    Int64Interval::UP interval {new Int64Interval(1939, 1945, 666)};

    // set and get
    int64_t value = -0x1234abcd9876fedcLL;
    interval->setValue(value);
    auto readValue = interval->getValue();
    CPPUNIT_ASSERT_EQUAL(value, readValue);
}

void Int64IntervalTest::testValueSerDes()
{
    Int64Interval::UP interval {new Int64Interval(1939, 1945, 666)};

    // set value
    int64_t value = -0x1234abcd9876fedcLL;
    interval->setValue(value);

    // make sure the variable data size is 8
    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(8), interval->getVariableDataSize());

    // serialize value in buffer
    std::uint8_t buf[8];
    interval->serializeVariableData(buf);

    // deserialize value in other interval
    Int64Interval::UP deserInterval {new Int64Interval(1939, 1945, 666)};
    deserInterval->deserializeVariableData(buf);

    // make sure both values are the same
    CPPUNIT_ASSERT_EQUAL(value, deserInterval->getValue());
}
