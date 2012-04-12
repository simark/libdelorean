/**
 * Copyright (c) 2012 François Rajotte <francois.rajotte@polymtl.ca>
 * Copyright (c) 2012 Philippe Proulx <philippe.proulx@polymtl.ca>
 *
 * This file is part of librbntrvll.
 *
 * librbntrvll is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * librbntrvll is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with librbntrvll.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <rbrntrvll/ThreadedHistoryTree.hpp>
#include <rbrntrvll/CoreNode.hpp>
#include <rbrntrvll/ex/TimeRangeEx.hpp>
#include <rbrntrvll/ex/IOEx.hpp>

#include <assert.h>

using namespace std;

ThreadedHistoryTree::ThreadedHistoryTree()
:AbstractHistoryTree(), HistoryTree(),
ThreadedInHistoryTree(), ThreadedOutHistoryTree()
{
}

/**
 * Create a new HistoryTree using a configuration
 * 
 * @param config
 */
ThreadedHistoryTree::ThreadedHistoryTree(HistoryTreeConfig config)
:AbstractHistoryTree(config), HistoryTree(config),
ThreadedInHistoryTree(config), ThreadedOutHistoryTree(config)
{
}

ThreadedHistoryTree::~ThreadedHistoryTree()
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
void ThreadedHistoryTree::open()
{
	HistoryTree::open();
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
void ThreadedHistoryTree::open(OpenMode mode)
{	
	HistoryTree::open(mode);
	this->startThread();
}

/**
 * "Save" the tree to disk.
 * If the specified end time is smaller than the greatest timestamp 
 * in the tree, it will be ajusted to be equal to the greatest timestamp
 * @param end : the desired end time of the tree
 */
void ThreadedHistoryTree::close(timestamp_t end)
{
	ThreadedOutHistoryTree::close(end);
}

/**
 * "Save" the tree to disk.
 * The greatest timestamp in the tree is used as the end time.
 */
void ThreadedHistoryTree::close(void)
{
	close(getEnd());
}

/**
 * It is necessary to specify which parent method to call because
 * both the threaded and non-threaded version of the history tree
 * are parents of this class.
 */ 
vector<AbstractInterval::SharedPtr> ThreadedHistoryTree::query(timestamp_t timestamp) const {
	return ThreadedInHistoryTree::query(timestamp);	
}

/**
 * It is necessary to specify which parent method to call because
 * both the threaded and non-threaded version of the history tree
 * are parents of this class.
 */ 
AbstractInterval::SharedPtr ThreadedHistoryTree::query(timestamp_t timestamp, attribute_t key) const {
	return ThreadedInHistoryTree::query(timestamp, key);
}
/**
 * It is necessary to specify which parent method to call because
 * both the threaded and non-threaded version of the history tree
 * are parents of this class.
 */ 
void ThreadedHistoryTree::addInterval(AbstractInterval::SharedPtr interval) throw(TimeRangeEx) {
	ThreadedOutHistoryTree::addInterval(interval);
}
/*
OutHistoryTree& ThreadedHistoryTree::operator<<(AbstractInterval::SharedPtr interval) throw(TimeRangeEx) {
	this->addInterval(interval);
	
	return *(ThreadedOutHistoryTree*)this;
}


*/
