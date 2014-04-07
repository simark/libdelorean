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
    CPPUNIT_ASSERT(interval->getValue() == value);

    // C string
    interval->setValue(value.c_str());
    CPPUNIT_ASSERT(interval->getValue() == value);
}

void StringIntervalTest::testSize()
{
    StringInterval::UP interval {new StringInterval(1, 2, 3)};
    std::string value("hello, world!");
    interval->setValue(value);

    // variable size: string length + NUL character
    std::size_t expectedVariableSize = value.size() + 1;
    CPPUNIT_ASSERT_EQUAL(interval->getVariableSize(), expectedVariableSize);

    // size should be length of variable data + header size (24)
    CPPUNIT_ASSERT_EQUAL(interval->getSize(), expectedVariableSize + 24);

    // no value: NUL byte only
    interval->setValue("");
    CPPUNIT_ASSERT_EQUAL(interval->getSize(), static_cast<std::size_t>(24 + 1));
}

void StringIntervalTest::testValueSerialization()
{
    // allocate and initialize a buffer
    std::unique_ptr<std::uint8_t[]> bufUp {new std::uint8_t [1024]};
    auto buf = bufUp.get();
    for (unsigned int x = 0; x < 1024; ++x) {
        buf[x] = x & 0xff;
    }

    // create interval with custom string value
    StringInterval::UP interval {new StringInterval(1608, 2008, 5)};
    std::string value("how are you?");
    interval->setValue(value);

    // serialize values
    auto varAt = 512;
    interval->serializeValues(buf, buf + varAt - interval->getVariableSize());

    // check serialized value
    for (unsigned int x = 0; x < value.size(); ++x) {
        auto offset = varAt - interval->getVariableSize() + x;
        CPPUNIT_ASSERT_EQUAL(buf[offset], static_cast<uint8_t>(value[x]));
    }
    CPPUNIT_ASSERT_EQUAL(buf[varAt - 1], static_cast<uint8_t>('\0'));

    // make sure everything else in the buffer didn't change
    for (unsigned int x = 0; x < varAt - interval->getVariableSize(); ++x) {
        CPPUNIT_ASSERT_EQUAL(buf[x], static_cast<uint8_t>(x & 0xff));
    }
    for (unsigned int x = varAt + interval->getVariableSize(); x < 1024; ++x) {
        CPPUNIT_ASSERT_EQUAL(buf[x], static_cast<uint8_t>(x & 0xff));
    }
}

void StringIntervalTest::testValueDeserialization()
{
    // allocate and initialize a buffer with a value
    std::unique_ptr<std::uint8_t[]> bufUp {new std::uint8_t [1024]};
    auto buf = bufUp.get();
    for (unsigned int x = 0; x < 1024; ++x) {
        buf[x] = x & 0xff;
    }

    // put some string at some offset
    auto varAt = 256;
    std::string value {"Bacon ipsum dolor sit amet bresaola porchetta pork pork"};
    std::memcpy(buf + varAt, value.c_str(), value.size());
    buf[varAt + value.size()] = '\0';
    std::uint32_t offsetFromEnd = 1024 - varAt;
    std::memcpy(buf, &offsetFromEnd, 4);

    // create interval
    StringInterval::UP interval {new StringInterval(1608, 2008, 5)};

    // deserialize values
    interval->deserializeValues(buf, buf + 1024);

    // check deserialized value
    CPPUNIT_ASSERT_EQUAL(value, interval->getValue());
}
