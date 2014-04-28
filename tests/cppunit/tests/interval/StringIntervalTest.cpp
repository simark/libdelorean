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
#include <string>

#include <delorean/interval/StringInterval.hpp>
#include <delorean/BasicTypes.hpp>
#include "StringIntervalTest.hpp"

CPPUNIT_TEST_SUITE_REGISTRATION(StringIntervalTest);

void StringIntervalTest::testConstructorValue()
{
    StringInterval::UP interval {new StringInterval(1, 2, 3)};
    std::string value {"bonjour"};

    // std::string
    interval->setValue(value);
    CPPUNIT_ASSERT_EQUAL(value, interval->getValue());
}

void StringIntervalTest::testVariableDataSize()
{
    StringInterval::UP interval {new StringInterval(1, 2, 3)};
    std::string value {"hello, world!"};
    interval->setValue(value);

    // variable size: string length + NUL character
    std::size_t expectedVariableSize = value.size() + 1;
    CPPUNIT_ASSERT_EQUAL(expectedVariableSize, interval->getVariableDataSize());

    // no variable data: NUL character only
    interval->setValue("");
    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(1), interval->getVariableDataSize());
}

void StringIntervalTest::testVariableDataSerialization()
{
    // allocate and initialize a buffer
    std::unique_ptr<std::uint8_t[]> bufUp {new std::uint8_t [1024]};
    auto buf = bufUp.get();
    for (std::size_t x = 0; x < 1024; ++x) {
        buf[x] = x & 0xff;
    }

    // create interval with custom string value
    StringInterval::UP interval {new StringInterval(1608, 2008, 5)};
    std::string value {"how are you?"};
    interval->setValue(value);

    // serialize values
    std::size_t varAt = 512;
    interval->serializeVariableData(buf + varAt);

    // check serialized value
    for (std::size_t x = 0; x < value.size(); ++x) {
        auto offset = varAt + x;
        CPPUNIT_ASSERT_EQUAL(static_cast<uint8_t>(value[x]), buf[offset]);
    }
    CPPUNIT_ASSERT_EQUAL(static_cast<uint8_t>('\0'), buf[varAt + value.size()]);

    // make sure everything else in the buffer didn't change
    for (std::size_t x = 0; x < varAt; ++x) {
        CPPUNIT_ASSERT_EQUAL(static_cast<uint8_t>(x & 0xff), buf[x]);
    }
    for (std::size_t x = varAt + interval->getVariableDataSize(); x < 1024; ++x) {
        CPPUNIT_ASSERT_EQUAL(static_cast<uint8_t>(x & 0xff), buf[x]);
    }
}

void StringIntervalTest::testVariableDataDeserialization()
{
    // allocate and initialize a buffer with a value
    std::unique_ptr<std::uint8_t[]> bufUp {new std::uint8_t [1024]};
    auto buf = bufUp.get();
    for (std::size_t x = 0; x < 1024; ++x) {
        buf[x] = x & 0xff;
    }

    // put some string at some offset
    std::size_t varAt = 256;
    std::string value {"Bacon ipsum dolor sit amet bresaola porchetta pork pork"};
    std::memcpy(buf + varAt, value.c_str(), value.size());
    buf[varAt + value.size()] = '\0';

    // create interval
    StringInterval::UP interval {new StringInterval(1608, 2008, 5)};

    // deserialize values
    interval->deserializeVariableData(buf + varAt);

    // check deserialized value
    CPPUNIT_ASSERT_EQUAL(value, interval->getValue());
}
