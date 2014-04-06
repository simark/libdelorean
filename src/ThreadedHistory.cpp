/**
 * Copyright (c) 2012 François Rajotte <francois.rajotte@polymtl.ca>
 * Copyright (c) 2012 Philippe Proulx <philippe.proulx@polymtl.ca>
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
#include <delorean/ThreadedHistory.hpp>
#include <delorean/CoreNode.hpp>
#include <delorean/ex/TimeRangeEx.hpp>
#include <delorean/ex/IOEx.hpp>

#include <assert.h>

using namespace std;

 /**
  * Constructs a threaded history tree, using a default configuration
  * 
  * @param maxQueueSize the maximum allowed size for the insertion queue
  */ 
ThreadedHistory::ThreadedHistory(unsigned int maxQueueSize)
:AbstractHistory(), History(),
ThreadedInHistory(), ThreadedOutHistory(maxQueueSize)
{
}

 /**
  * Constructs a threaded history tree, using the given configuration
  * 
  * @param config A configuration
  * @param maxQueueSize the maximum allowed size for the insertion queue
  */ 
ThreadedHistory::ThreadedHistory(HistoryConfig config, unsigned int maxQueueSize)
:AbstractHistory(config), History(config),
ThreadedInHistory(config), ThreadedOutHistory(config, maxQueueSize)
{
}

ThreadedHistory::~ThreadedHistory()
{
}

/**
 * Open a tree from the disk
 * First, tries to open an existing file
 * If the format is invalid, create an empty tree and overwrite the file
 * If the format is valid, read the file and init the tree
 * 
 * Second, tries to create an empty tree and a new file
 * @throw IOEx 
 * @throw InvalidFormatEx
 */
void ThreadedHistory::open()
{
	History::open();
	this->startThread();
}

/**
 * This version of "open" is specific to the input/output history
 * tree.
 * It allows to specify if the contents of the file should be :
 * - read first and kept (APPEND)
 * - discarded (TRUNCATE)
 * 
 * Using the APPEND mode requires the file to already exist or else
 * an exception will be thrown.
 * 
 * @param mode either APPEND (keep existing file) or TRUNCATE (replace existing file)
 * @throw IOEx
 */
void ThreadedHistory::open(OpenMode mode)
{	
	History::open(mode);
	this->startThread();
}

/**
 * "Save" the tree to disk.
 * If the specified end time is smaller than the greatest timestamp 
 * in the tree, it will be ajusted to be equal to the greatest timestamp
 * @param end : the desired end time of the tree
 */
void ThreadedHistory::close(timestamp_t end)
{
	ThreadedOutHistory::close(end);
}

/**
 * "Save" the tree to disk.
 * The greatest timestamp in the tree is used as the end time.
 */
void ThreadedHistory::close(void)
{
	close(getEnd());
}

/**
 * It is necessary to specify which parent method to call because
 * both the threaded and non-threaded version of the history tree
 * are parents of this class.
 */ 
vector<AbstractInterval::SharedPtr> ThreadedHistory::query(timestamp_t timestamp) const {
	return ThreadedInHistory::query(timestamp);	
}

/**
 * It is necessary to specify which parent method to call because
 * both the threaded and non-threaded version of the history tree
 * are parents of this class.
 */ 
AbstractInterval::SharedPtr ThreadedHistory::query(timestamp_t timestamp, attribute_t key) const {
	return ThreadedInHistory::query(timestamp, key);
}

/**
 * It is necessary to specify which parent method to call because
 * both the threaded and non-threaded version of the history tree
 * are parents of this class.
 */ 
multimap<attribute_t, AbstractInterval::SharedPtr> ThreadedHistory::sparseQuery(timestamp_t timestamp) const {
	return ThreadedInHistory::sparseQuery(timestamp);
}
/**
 * It is necessary to specify which parent method to call because
 * both the threaded and non-threaded version of the history tree
 * are parents of this class.
 */ 
void ThreadedHistory::addInterval(AbstractInterval::SharedPtr interval) throw(TimeRangeEx) {
	ThreadedOutHistory::addInterval(interval);
}
