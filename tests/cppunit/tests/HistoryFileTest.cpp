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
    std::unique_ptr<HistoryFileSink> hfSink {new HistoryFileSink {}};

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
    std::unique_ptr<HistoryFileSink> hfSink {new HistoryFileSink {}};

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

void HistoryFileTest::testBuildEmpty()
{
    // create history file sink
    std::unique_ptr<HistoryFileSink> hfSink {new HistoryFileSink {}};

    // open
    hfSink->open("./history.his", 8192, 16);
    CPPUNIT_ASSERT(hfSink->isOpened());

    // close
    hfSink->close();

    /* The file created should have a size of (header size + node size) bytes,
     * or 12288 bytes. This is because only one node should be created.
     */
    CPPUNIT_ASSERT_EQUAL(bfs::file_size("./history.his"), static_cast<uintmax_t>(12288));
}

void HistoryFileTest::testAddIntervals()
{
    // interval jar
    std::unique_ptr<IntervalJar> jar {new IntervalJar {}};

    // fill jar with intervals
    getIntervalsFromTextFile("../data/headsofstates.txt", *jar);

    // create history file sink and open it
    std::unique_ptr<HistoryFileSink> hfSink {new HistoryFileSink};
    hfSink->open("./history.his", 4096, 16);

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
}
