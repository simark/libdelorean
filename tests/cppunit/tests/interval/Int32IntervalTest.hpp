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
#ifndef _INT32INTERVALTEST_HPP
#define _INT32INTERVALTEST_HPP

#include <cppunit/extensions/HelperMacros.h>

class Int32IntervalTest :
    public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(Int32IntervalTest);
        CPPUNIT_TEST(testConstructorAndAttributes);
        CPPUNIT_TEST(testIntersection);
        CPPUNIT_TEST(testOperators);
        CPPUNIT_TEST(testFixedValue);
        CPPUNIT_TEST(testVariableDataSize);
    CPPUNIT_TEST_SUITE_END();

public:
    void testConstructorAndAttributes();
    void testIntersection();
    void testOperators();
    void testFixedValue();
    void testVariableDataSize();
};

#endif // _INT32INTERVALTEST_HPP
