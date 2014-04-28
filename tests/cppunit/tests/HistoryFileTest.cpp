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
#include <boost/filesystem.hpp>

#include <delorean/HistoryFileSink.hpp>
#include <delorean/HistoryFileSource.hpp>
#include <delorean/BasicTypes.hpp>
#include <delorean/interval/IntInterval.hpp>
#include <delorean/interval/StringInterval.hpp>
#include <delorean/interval/IntervalJar.hpp>
#include <delorean/ex/IO.hpp>
#include <delorean/ex/TimestampOutOfRange.hpp>
#include <utils.hpp>
#include "HistoryFileTest.hpp"

namespace bfs = boost::filesystem;

CPPUNIT_TEST_SUITE_REGISTRATION(HistoryFileTest);

void HistoryFileTest::testNonExistingFile()
{
    // create history file sink
    std::unique_ptr<HistoryFileSink> hfSink {new HistoryFileSink};

    // try opening a non existing file
    try {
        hfSink->open("/this/path/should/not/exist");
        CPPUNIT_FAIL("Opened a history file sink with a non existing file");
    } catch (const IO& ex) {
    }

    CPPUNIT_ASSERT(!hfSink->isOpened());
}

void HistoryFileTest::testAddIntervalWhenClosed()
{
    // create history file sink
    std::unique_ptr<HistoryFileSink> hfSink {new HistoryFileSink};

    // add interval without opening
    IntInterval::SP interval {new IntInterval(1939, 1945, 1)};
    try {
        hfSink->addInterval(interval);
        CPPUNIT_FAIL("Adding an interval to a closed history file sink");
    } catch (const IO& ex) {
    }

    // close
    hfSink->close();
}

void HistoryFileTest::testQueryWhenClosed()
{
    // create history file source
    std::unique_ptr<HistoryFileSource> hfSource {new HistoryFileSource};

    // add interval without opening
    try {
        IntervalJar jar;
        hfSource->findAll(1923, jar);
        CPPUNIT_FAIL("Querying a closed history file sink");
    } catch (const IO& ex) {
    }

    // close
    hfSource->close();
}

void HistoryFileTest::testBuildEmpty()
{
    // create history file sink
    std::unique_ptr<HistoryFileSink> hfSink {new HistoryFileSink};

    // open
    hfSink->open("./history.his", 8192, 16, 1917);
    CPPUNIT_ASSERT(hfSink->isOpened());

    // close
    hfSink->close();

    /* The file created should have a size of (header size + node size) bytes,
     * or 12288 bytes. This is because only one node should be created.
     */
    CPPUNIT_ASSERT_EQUAL(bfs::file_size("./history.his"),
                         static_cast<uintmax_t>(12288));

    // create and open history file source
    std::unique_ptr<HistoryFileSource> hfSource {new HistoryFileSource};
    hfSource->open("./history.his");

    // begin AND end should be 1917 (no intervals added)
    CPPUNIT_ASSERT_EQUAL(hfSource->getBegin(), static_cast<timestamp_t>(1917));
    CPPUNIT_ASSERT_EQUAL(hfSource->getEnd(), static_cast<timestamp_t>(1917));

    // any query outside 1917 should throw
    IntervalJar jar;
    try {
        hfSource->findAll(1916, jar);
        CPPUNIT_FAIL("Queried history file source before its range");
    } catch (const TimestampOutOfRange& ex) {
        CPPUNIT_ASSERT_EQUAL(ex.getBegin(), static_cast<timestamp_t>(1917));
        CPPUNIT_ASSERT_EQUAL(ex.getEnd(), static_cast<timestamp_t>(1917));
        CPPUNIT_ASSERT_EQUAL(ex.getTs(), static_cast<timestamp_t>(1916));
    }
    CPPUNIT_ASSERT(jar.empty());
    try {
        hfSource->findAll(1918, jar);
        CPPUNIT_FAIL("Queried history file source after its range");
    } catch (const TimestampOutOfRange& ex) {
        CPPUNIT_ASSERT_EQUAL(ex.getBegin(), static_cast<timestamp_t>(1917));
        CPPUNIT_ASSERT_EQUAL(ex.getEnd(), static_cast<timestamp_t>(1917));
        CPPUNIT_ASSERT_EQUAL(ex.getTs(), static_cast<timestamp_t>(1918));
    }
    CPPUNIT_ASSERT(jar.empty());

    // we may query 1917, but it shouldn't return anything
    auto res = hfSource->findAll(1917, jar);
    CPPUNIT_ASSERT(!res);
    CPPUNIT_ASSERT(jar.empty());

    // close history file source
    hfSource->close();
}

void HistoryFileTest::testAddFindIntervals()
{
    // interval jar
    std::unique_ptr<IntervalJar> jar {new IntervalJar {}};

    // fill jar with intervals
    getIntervalsFromTextFile("../data/headsofstates.txt", *jar);

    // create history file sink and open it
    std::unique_ptr<HistoryFileSink> hfSink {new HistoryFileSink};
    hfSink->open("./history.his", 1024, 16, 15123456);

    // add intervals
    for (auto& interval : *jar) {
        try {
            hfSink->addInterval(interval);
        } catch (const std::exception& ex) {
            CPPUNIT_FAIL("Adding an interval threw an exception");
        }
    }

    // close history file sink
    try {
        hfSink->close();
    } catch (const std::exception& ex) {
        CPPUNIT_FAIL("Closing the history file threw an exception");
    }

    // create history file source and open it
    std::unique_ptr<HistoryFileSource> hfSource {new HistoryFileSource};
    hfSource->open("./history.his");

    // verify some properties
    CPPUNIT_ASSERT_EQUAL(hfSource->getBegin(), static_cast<timestamp_t>(15123456));

    // prepare queries
    IntervalJar tmpJar;
    bool res;

    // first interval starts at 17210404; try before that
    res = hfSource->findAll(17210403, tmpJar);
    CPPUNIT_ASSERT(!res);
    CPPUNIT_ASSERT(tmpJar.empty());
    res = hfSource->findAll(17210401, tmpJar);
    CPPUNIT_ASSERT(!res);
    CPPUNIT_ASSERT(tmpJar.empty());
    res = hfSource->findAll(16080703, tmpJar);
    CPPUNIT_ASSERT(!res);
    CPPUNIT_ASSERT(tmpJar.empty());

    // try getting first interval only (17210404)
    res = hfSource->findAll(17210404, tmpJar);
    CPPUNIT_ASSERT(res);
    CPPUNIT_ASSERT_EQUAL(tmpJar.size(), static_cast<std::size_t>(1));
    auto onlyInterval = tmpJar.at(0);
    CPPUNIT_ASSERT_EQUAL(onlyInterval->getBegin(), static_cast<timestamp_t>(17210404));
    CPPUNIT_ASSERT_EQUAL(onlyInterval->getEnd(), static_cast<timestamp_t>(17420211));
    CPPUNIT_ASSERT_EQUAL(onlyInterval->getKey(), static_cast<interval_key_t>(4));
    CPPUNIT_ASSERT_EQUAL(static_cast<StringInterval&>(*onlyInterval).getValue(),
                         std::string {"Sir Robert Walpole"});

    // 1820
    res = hfSource->findAll(17210404, tmpJar);
    CPPUNIT_ASSERT_EQUAL(tmpJar.size(), static_cast<std::size_t>(2));

    // close history file source
    hfSource->close();
}
