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
#include "ThreadedHistoryTree.hpp"
#include "CoreNode.hpp"
#include "ex/TimeRangeEx.hpp"
#include "ex/IOEx.hpp"

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
 * This method will cause the treeIO object to commit all nodes to disk
 * and then return the RandomAccessFile descriptor so the Tree object can
 * save its configuration into the header of the file.
 * @throws TimeRangeException 
 * 
 */
void ThreadedHistoryTree::close(timestamp_t end)
{
	ThreadedOutHistoryTree::close(end);
}

void ThreadedHistoryTree::close(void)
{
	close(getEnd());
}
