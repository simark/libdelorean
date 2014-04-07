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
#ifndef INTINTERVALTEST_HPP
#define INTINTERVALTEST_HPP

#include <cppunit/extensions/HelperMacros.h>
#include <delorean/interval/IntInterval.hpp>

class IntIntervalTest :
    public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(IntIntervalTest);
        CPPUNIT_TEST(testConstructorAndAttributes);
        CPPUNIT_TEST(testIntersection);
        CPPUNIT_TEST(testOperators);
        CPPUNIT_TEST(testSize);
        CPPUNIT_TEST(testValueSerialization);
        CPPUNIT_TEST(testValueDeserialization);
    CPPUNIT_TEST_SUITE_END();

public:
    void testConstructorAndAttributes();
    void testIntersection();
    void testOperators();
    void testSize();
    void testValueSerialization();
    void testValueDeserialization();
};

#endif  // INTINTERVALTEST_HPP
