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
#ifndef INTERVALSERIALIZERTEST_HPP
#define INTERVALSERIALIZERTEST_HPP

#include <cppunit/extensions/HelperMacros.h>
#include <delorean/interval/IntInterval.hpp>

class IntervalSerializerTest :
    public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(IntervalSerializerTest);
        CPPUNIT_TEST(testSerialization);
    CPPUNIT_TEST_SUITE_END();

public:
    void testSerialization();
};

#endif  // INTERVALSERIALIZERTEST_HPP
