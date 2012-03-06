/**
 * Copyright (c) 2012 François Rajotte <francois.rajotte@polymtl.ca>
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
#include "HistoryTree.hpp"
#include "HistoryTreeCoreNode.hpp"
#include "ex/TimeRangeEx.hpp"
#include "ex/IOEx.hpp"

#include <assert.h>

using namespace std;

/**
 * Create a new HistoryTree using a configuration
 * 
 * @param config
 */
HistoryTree::HistoryTree(HistoryTreeConfig config)
:AbstractHistoryTree(config), InHistoryTree(config), OutHistoryTree(config)
{
}

HistoryTree::~HistoryTree()
{
}

/**
 * Open a tree from the disk
 * @throws TimeRangeException 
 * 
 */
void HistoryTree::open(void)
{
	// is this history tree already opened?
	if (this->_opened) {
		throw IOEx("This tree is already opened");
	}
	
	// open stream
	this->_stream.open(this->_config._stateFile.c_str(), fstream::in | fstream::out | fstream::binary);
	
	// check for open errors
	if (!this->_stream) {
		//The file did not exist, create a new file
		this->_stream.open(this->_config._stateFile.c_str(), fstream::in | fstream::out | fstream::binary | fstream::trunc);
		
		if (!this->_stream) {
			throw IOEx("Unable to open file");
		}
		initEmptyTree();
	
		// update internal status
		this->_opened = true;
		return;
	}
	
	try{
		// unserialize tree header
		this->unserializeHeader();
	}catch(IOEx& ex){
		//Unable to read header, open it as an empty tree
		initEmptyTree();
		
		// update internal status
		this->_opened = true;
		return;
	}
	//We read the header correctly, init the tree
	
	// store latest branch in memory
	this->buildLatestBranch();
	
	// The user could not possibly know the start and end times of the tree
	// Set it to the correct value using the root node
	_config._treeStart = _latest_branch[0]->getStart();
	_end = _latest_branch[0]->getEnd();
	
	// update internal status
	this->_opened = true;
}

/**
 * "Save" the tree to disk.
 * This method will cause the treeIO object to commit all nodes to disk
 * and then return the RandomAccessFile descriptor so the Tree object can
 * save its configuration into the header of the file.
 * @throws TimeRangeException 
 * 
 */
void HistoryTree::close(timestamp_t end)
{
	// is this history tree at least opened?
	if (!this->_opened) {
		throw IOEx("This tree was not open");
	}
	
	// proper end time
	if (end < this->_end) {
		end = this->_end;
	}
	
	// close the latest branch
	for (unsigned int i = 0; i < this->_latest_branch.size(); ++i) {
		this->_latest_branch[i]->close(this->_end);
		this->serializeNode(this->_latest_branch[i]);
	}
	
	// write tree header
	this->serializeHeader();
	
	// close stream
	this->closeStream();
	
	// update internal status
	this->_opened = false;
}

void HistoryTree::close(void)
{
	close(getEnd());
}
