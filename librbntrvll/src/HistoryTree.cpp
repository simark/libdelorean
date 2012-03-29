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
 * First, tries to open an existing file
 * If the format is invalid, create an empty tree and overwrite the file
 * If the format is valid, read the file and init the tree
 * 
 * Second, tries to create an empty tree and a new file
 * @throw IOEx 
 * @throw InvalidFormatEx
 */
void HistoryTree::open()
{
	// is this history tree already opened?
	if (this->_opened) {
		throw IOEx("This tree is already opened");
	}
	
	try{
		open(APPEND);
	}catch(InvalidFormatEx& ex){
		throw;
	}catch(IOEx& ex){
		try{
			open(TRUNCATE);
		}catch(IOEx& ex){
			throw;
		}
	}
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
void HistoryTree::open(OpenMode mode)
{	
	// is this history tree already opened?
	if (this->_opened) {
		throw IOEx("This tree is already opened");
	}
	
	if(mode == TRUNCATE){
		// open stream
		this->_stream.open(this->_config._stateFile.c_str(), fstream::in | fstream::out | fstream::binary | fstream::trunc);
		
		// check for open errors
		if (!this->_stream) {
			throw IOEx("Unable to open file");
		}
		initEmptyTree();
	
		// update internal status
		this->_opened = true;
		
	}else if (mode == APPEND){
		// open stream
		this->_stream.open(this->_config._stateFile.c_str(), fstream::in | fstream::out | fstream::binary);
		
		// check for open errors
		if (!this->_stream) {
			throw IOEx("Unable to open file");
		}
		
		// check if empty file
		this->_stream.seekg(ios_base::end);
		if (this->_stream.tellg() == 0){
			this->_stream.close();
			throw IOEx("This file is empty");
		}
		
		
		try{
			// unserialize tree header
			this->unserializeHeader();
		}catch(InvalidFormatEx& ex){	
			this->_stream.close();		
			throw;
		}catch(IOEx& ex){
			this->_stream.close();
			throw;
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
		
	}else{
		assert(false);
	}
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
