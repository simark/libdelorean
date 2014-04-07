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
#include <typeinfo>

#include <delorean/interval/IntervalDeserializer.hpp>
#include <delorean/interval/StandardIntervalTypes.hpp>
#include <delorean/interval/SimpleIntervalFactory.hpp>
#include <delorean/interval/IntInterval.hpp>
#include <delorean/interval/StringInterval.hpp>
#include <delorean/ex/ExistingIntervalType.hpp>
#include <delorean/BasicTypes.hpp>
#include "IntervalDeserializerTest.hpp"

CPPUNIT_TEST_SUITE_REGISTRATION(IntervalDeserializerTest);

void IntervalDeserializerTest::testIntervalTypeRegistration()
{
    // build interval deserializer
    std::unique_ptr<IntervalDeserializer> deser {new IntervalDeserializer()};

    // build some interval factory
    auto intFactoryPtr = new SimpleIntervalFactory<IntInterval> {};
    IIntervalFactory::UP intFactory {intFactoryPtr};

    // try registering it using an existing type
    auto type = static_cast<interval_type_t>(StandardIntervalTypes::STRING);
    try {
        deser->registerIntervalType(type, std::move(intFactory));
        CPPUNIT_FAIL("Registered a type already registered");
    } catch (const ExistingIntervalType& ex) {
    }

    // try registering it using a non-existing type
    try {
        deser->registerIntervalType(155, std::move(intFactory));
    } catch (const ExistingIntervalType& ex) {
        CPPUNIT_FAIL("Cannot register unused type");
    }
}

void IntervalDeserializerTest::testDeserialization()
{
    // we only test deserialization of a known type here.

    // build interval deserializer
    std::unique_ptr<IntervalDeserializer> deser {new IntervalDeserializer()};

    // build buffer
    std::unique_ptr<std::uint8_t[]> bufUp {new std::uint8_t [1024]};
    auto buf = bufUp.get();
    for (unsigned int x = 0; x < 1024; ++x) {
        buf[x] = x & 0xff;
    }

    // write string interval in buffer
    std::string strValue {"std::string strValue"};
    interval_type_t strType = static_cast<interval_type_t>(StandardIntervalTypes::STRING);
    struct head
    {
        timestamp_t begin;
        timestamp_t end;
        uint32_t typeId;
        uint32_t value;
    };
    head stringHead = {
        .begin = 1534,
        .end = 1608,
        .typeId = static_cast<uint32_t>(strType << 24) | 53,
        .value = static_cast<uint32_t>(strValue.size() + 1)
    };
    std::memcpy(buf, &stringHead, 24);
    std::memcpy(buf + 1024 - strValue.size() - 1, strValue.c_str(), strValue.size());
    buf[1023] = '\0';

    // deserialize (get interval)
    auto abstractIntervalUp = deser->deserializeInterval(buf, buf + 1024);
    AbstractInterval& abstractInterval = *abstractIntervalUp;

    // cast to string interval
    try {
        // should work if the right interval type was created
        StringInterval& strInterval = dynamic_cast<StringInterval&>(abstractInterval);

        // verify deserialized values
        CPPUNIT_ASSERT_EQUAL(strInterval.getBegin(), stringHead.begin);
        CPPUNIT_ASSERT_EQUAL(strInterval.getEnd(), stringHead.end);
        CPPUNIT_ASSERT_EQUAL(strInterval.getType(), strType);
        CPPUNIT_ASSERT_EQUAL(strInterval.getId(), static_cast<interval_id_t>(53));
        CPPUNIT_ASSERT_EQUAL(strInterval.getValue(), strValue);
    } catch (std::bad_cast& ex) {
        CPPUNIT_FAIL("Deserializer did not create the right interval type");
    }
}
