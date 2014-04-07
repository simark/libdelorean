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
#include <string>

#include <delorean/interval/IntervalSerializer.hpp>
#include <delorean/interval/IntInterval.hpp>
#include <delorean/interval/StringInterval.hpp>
#include <delorean/interval/StandardIntervalTypes.hpp>
#include <delorean/BasicTypes.hpp>
#include "IntervalSerializerTest.hpp"

CPPUNIT_TEST_SUITE_REGISTRATION(IntervalSerializerTest);

void IntervalSerializerTest::testSerialization()
{
    /* We will serialize two one string interval and then one int interval
     * and verify if they were correctly serialized and at the right place.
     */

    // allocate and initialize a buffer
    std::unique_ptr<std::uint8_t[]> bufUp {new std::uint8_t [1024]};
    auto buf = bufUp.get();
    for (unsigned int x = 0; x < 1024; ++x) {
        buf[x] = x & 0xff;
    }

    // string interval
    StringInterval::UP interval1 {new StringInterval(1, 2, 3)};
    std::string stringValue {"Bacon ipsum dolor sit amet bresaola porchetta pork pork"};
    interval1->setValue(stringValue);

    // int interval
    IntInterval::UP interval2 {new IntInterval(1534, 1608, 55)};
    int intValue = 0x12ab34cd;
    interval2->setValue(intValue);

    // create interval serializer
    std::unique_ptr<IntervalSerializer> ser {new IntervalSerializer()};

    // serialize string interval
    ser->serializeInterval(*interval1, buf,
                           buf + 1024 - interval1->getVariableSize(),
                           interval1->getVariableSize());

    // serialize int interval
    ser->serializeInterval(*interval2, buf + 24, nullptr,
                           interval2->getVariableSize());

    // read serialized data
    struct head
    {
        std::uint64_t begin;
        std::uint64_t end;
        std::uint32_t typeId;
        std::uint32_t value;
    };
    head stringHead;
    head intHead;
    std::memcpy(&stringHead, buf, 24);
    std::memcpy(&intHead, buf + 24, 24);

    // verify serialized headers
    uint32_t expectedTypeId;
    uint32_t expectedValue;

    CPPUNIT_ASSERT_EQUAL(stringHead.begin, static_cast<std::uint64_t>(1));
    CPPUNIT_ASSERT_EQUAL(stringHead.end, static_cast<std::uint64_t>(2));
    expectedTypeId = static_cast<std::uint32_t>(StandardIntervalTypes::STRING) << 24 | 3;
    CPPUNIT_ASSERT_EQUAL(stringHead.typeId, expectedTypeId);
    expectedValue = interval1->getVariableSize();
    CPPUNIT_ASSERT_EQUAL(stringHead.value, expectedValue);

    CPPUNIT_ASSERT_EQUAL(intHead.begin, static_cast<std::uint64_t>(1534));
    CPPUNIT_ASSERT_EQUAL(intHead.end, static_cast<std::uint64_t>(1608));
    expectedTypeId = static_cast<std::uint32_t>(StandardIntervalTypes::INT32) << 24 | 55;
    CPPUNIT_ASSERT_EQUAL(intHead.typeId, expectedTypeId);
    expectedValue = intValue;
    CPPUNIT_ASSERT_EQUAL(intHead.value, expectedValue);

    // verify serialized string
    char* strPtr = reinterpret_cast<char*>(buf + 1024 - interval1->getVariableSize());

    // this is safe because serialized string should end with a NUL character
    std::string expectedStringValue(strPtr);
    CPPUNIT_ASSERT_EQUAL(stringValue, expectedStringValue);
}
