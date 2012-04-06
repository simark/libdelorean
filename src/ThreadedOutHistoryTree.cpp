/**
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
#include <fstream>
#include <ostream>
#include <cassert>
#include <tr1/memory>
#include <cstdlib>
 
#include "ThreadedOutHistoryTree.hpp"
#include "intervals/AbstractInterval.hpp"
#include "HistoryTreeConfig.hpp"
#include "CoreNode.hpp"
#include "LeafNode.hpp"
#include "ex/IOEx.hpp"
#include "fixed_config.h"

#include "intervals/NullInterval.hpp"
 
using namespace std;
using namespace std::tr1;
 
ThreadedOutHistoryTree::ThreadedOutHistoryTree()
: AbstractHistoryTree() , OutHistoryTree(), AbstractThreadedHistoryTree(){
}

ThreadedOutHistoryTree::ThreadedOutHistoryTree(HistoryTreeConfig config)
: AbstractHistoryTree(config) , OutHistoryTree(config), AbstractThreadedHistoryTree(config){
}

/**
 * Opens a history tree file for writing only.
 * If the file already existed, it will be overwritten
 * 
 * @throw IOEx if file already open or general IO error
 */ 
void ThreadedOutHistoryTree::open() {
	
	OutHistoryTree::open();	
	this->startThread();
}

void ThreadedOutHistoryTree::close(timestamp_t end) {
	
	this->stopThread();
	OutHistoryTree::close(end);
}
/*
ThreadedOutHistoryTree::~ThreadedOutHistoryTree() {
	if (this->_opened) {
		OutHistoryTree::close();
	}
}*/

void ThreadedOutHistoryTree::addInterval(AbstractInterval::SharedPtr interval) throw(TimeRangeEx) {
	if (interval->getStart() < this->_config._treeStart) {
		throw TimeRangeEx("interval start time below tree start time");
	}
	
	boost::unique_lock<boost::mutex> lock(_insertQueue_mutex);
	bool const was_empty=_insertQueue.empty();
	_insertQueue.push(interval);
	if(was_empty)
	{
		_insertConditionVariable.notify_one();
	}
}

void ThreadedOutHistoryTree::addSiblingNode(unsigned int index) {
	AbstractNode::SharedPtr new_node;
	CoreNode::SharedPtr prev_node;
	timestamp_t split_time = this->_end;
	
	// make sure required index is within the latest branch
	assert(index < this->_latest_branch.size());
	
	// do we need a new root node?
	if (index == 0) {
		this->addNewRootNode();
		
		return;
	}
	
	// check if we can indeed add a child to the target parent (should be a core node...)
	CoreNode::SharedPtr parent = dynamic_pointer_cast<CoreNode>(this->_latest_branch[index - 1]);
	assert(parent.get() != NULL);
	if (parent->getNbChildren() == this->_config._maxChildren) {
		// we cannot: then, add a sibling one level higher in latest branch
		this->addSiblingNode(index - 1);
		
		return;
	}

	// split off the new branch from the old one
	boost::unique_lock<boost::shared_mutex> l(_latest_branch_mutex);
	for (unsigned int i = index; i < this->_latest_branch.size(); ++i) {
		this->_latest_branch[i]->close(split_time);
		this->serializeNode(this->_latest_branch[i]);
		
		// get parent node
		prev_node = dynamic_pointer_cast<CoreNode>(this->_latest_branch[i - 1]);
		assert(prev_node.get() != NULL);
		
		// is this a leaf node or a core node?
		if (i == this->_latest_branch.size() - 1) {
			new_node = this->initNewLeafNode(prev_node->getSequenceNumber(), split_time + 1);
		} else {
			new_node = this->initNewCoreNode(prev_node->getSequenceNumber(), split_time + 1);
		}
		
		// link the new child to its parent
		prev_node->linkNewChild(new_node);
		
		this->_latest_branch[i] = new_node;
	}
	return;
}

void ThreadedOutHistoryTree::initEmptyTree(void) {
	
	boost::unique_lock<boost::shared_mutex> l(_latest_branch_mutex);
	// do init. stuff...
	this->_end = this->_config._treeStart;
	this->_node_count = 0;
	this->_latest_branch.clear();
	
	// add a first (*leaf*) node
	LeafNode::SharedPtr n = this->initNewLeafNode(-1, this->_config._treeStart);
	_latest_branch.push_back(n);
}

void ThreadedOutHistoryTree::addNewRootNode(void) {
	unsigned int i, depth;
	AbstractNode::SharedPtr new_node;
	CoreNode::SharedPtr prev_node;
	timestamp_t split_time = this->_end;
	
	// configure old/new root nodes
	AbstractNode::SharedPtr old_root(this->_latest_branch[0]);
	CoreNode::SharedPtr new_root = initNewCoreNode(-1, this->_config._treeStart);
	
	// tell the old root node that it isn't root anymore :(
	old_root->setParentSequenceNumber(new_root->getSequenceNumber());
	
	// close off the whole current latest branch
	for (i = 0; i < this->_latest_branch.size(); ++i) {
		_latest_branch[i]->close(split_time);
		this->serializeNode(this->_latest_branch[i]);
	}
	
	// link the new root to its first child (the previous root node)
	new_root->linkNewChild(old_root);
	
	// rebuild a brand new latest branch
	boost::unique_lock<boost::shared_mutex> l(_latest_branch_mutex);
	depth = _latest_branch.size();
	_latest_branch.clear();
	_latest_branch.push_back(new_root);
	for (i = 1; i < depth + 1; ++i) {
		// get parent node
		prev_node = dynamic_pointer_cast<CoreNode>(this->_latest_branch[i - 1]);
		
		// is this a leaf node or a core node?
		if (i == depth) {
			new_node = this->initNewLeafNode(prev_node->getSequenceNumber(), split_time + 1);
		} else {
			new_node = this->initNewCoreNode(prev_node->getSequenceNumber(), split_time + 1);
		}
		
		// link the new child to its parent
		prev_node->linkNewChild(new_node);
		
		this->_latest_branch.push_back(new_node);
	}
}

void ThreadedOutHistoryTree::serializeNode(AbstractNode::SharedPtr node) {
	boost::unique_lock<boost::recursive_mutex> l(this->_stream_read_mutex);
	// seek to correct position
	this->_stream.seekp(this->filePosFromSeq(node->getSequenceNumber()));
	
	// serialize node as is
	node->serialize(this->_stream);
}

void ThreadedOutHistoryTree::manageInsert(void){
	bool poisoned = false;
	boost::unique_lock<boost::mutex> l(_insertQueue_mutex);
	while(!poisoned)
	{
		//Wait for data to process
		while(_insertQueue.empty()){
			_insertConditionVariable.wait(l);
		}
		
		// Process data
		while(!_insertQueue.empty()){
			AbstractInterval::SharedPtr interval = _insertQueue.front();
			_insertQueue.pop();
			
			// Unlock mutex for the actual insert
			l.unlock();
			
			// Check for poisoned data
			if (interval->getStart() < this->_config._treeStart){
				poisoned = true;
				break;
			}
			
			//Insert data in tree
			this->tryInsertAtNode(interval, this->_latest_branch.size() - 1);
			
			// Relock mutex for next pop
			l.lock();
		}
	}
}

void ThreadedOutHistoryTree::startThread(void){
	
	_insertThread = boost::thread(boost::bind(&ThreadedOutHistoryTree::manageInsert, this));
}

void ThreadedOutHistoryTree::stopThread(void){
	{
		boost::unique_lock<boost::mutex> lock(_insertQueue_mutex);
		bool const was_empty=_insertQueue.empty();
		_insertQueue.push(AbstractInterval::SharedPtr(new NullInterval(this->_config._treeStart-1, this->_config._treeStart-1, 0)));
		if(was_empty)
		{
			_insertConditionVariable.notify_one();
		}
	}
	_insertThread.join();	
}
