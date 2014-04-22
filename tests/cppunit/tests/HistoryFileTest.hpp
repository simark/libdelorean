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
#ifndef _HISTORYFILETEST_HPP
#define _HISTORYFILETEST_HPP

#include <cppunit/extensions/HelperMacros.h>

class HistoryFileTest :
    public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(HistoryFileTest);
        CPPUNIT_TEST(testNonExistingFile);
        CPPUNIT_TEST(testAddIntervalWhenClosed);
        CPPUNIT_TEST(testBuildEmpty);
        CPPUNIT_TEST(testAddIntervals);
    CPPUNIT_TEST_SUITE_END();

public:
    void testNonExistingFile();
    void testAddIntervalWhenClosed();
    void testBuildEmpty();
    void testAddIntervals();
};

#endif // _HISTORYFILETEST_HPP
