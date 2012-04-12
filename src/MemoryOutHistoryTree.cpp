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
 
#include <rbrntrvll/MemoryOutHistoryTree.hpp>
#include <rbrntrvll/intervals/AbstractInterval.hpp>
#include <rbrntrvll/HistoryTreeConfig.hpp>
#include <rbrntrvll/CoreNode.hpp>
#include <rbrntrvll/LeafNode.hpp>
#include <rbrntrvll/ex/IOEx.hpp>
#include <rbrntrvll/fixed_config.h>

#include <rbrntrvll/intervals/NullInterval.hpp>
 
using namespace std;
using namespace std::tr1;
 
MemoryOutHistoryTree::MemoryOutHistoryTree(bool writeOnClose)
: AbstractHistoryTree() , OutHistoryTree(), AbstractMemoryHistoryTree(), _writeOnClose(writeOnClose){
}

MemoryOutHistoryTree::MemoryOutHistoryTree(HistoryTreeConfig config, bool writeOnClose)
: AbstractHistoryTree(config) , OutHistoryTree(config), AbstractMemoryHistoryTree(config), _writeOnClose(writeOnClose){
}

/**
 * Opens a history tree file for writing only.
 * If the file already existed, it will be overwritten
 * 
 * @throw IOEx if file already open or general IO error
 */ 
void MemoryOutHistoryTree::open() {
	
	_nodes.clear();
	initEmptyTree();
	
	this->startThread();
	
	_opened = true;
}

void MemoryOutHistoryTree::close(timestamp_t end) {
	
	this->stopThread();
	
	// proper end time
	if (end < this->_end) {
		end = this->_end;
	}
	
	// close the latest branch
	for (unsigned int i = 0; i < this->_latest_branch.size(); ++i) {
		this->_latest_branch[i]->close(this->_end);
	}
	
	if(_writeOnClose) {
		writeToFile();
	}
	
	_opened = false;
}

void MemoryOutHistoryTree::writeToFile() {
	
	openStream();
	
	serializeHeader();
	for(unsigned int i = 0; i < this->_node_count; ++i){
		serializeNode(_nodes[i]);
	}
	
	closeStream();
}

MemoryOutHistoryTree::~MemoryOutHistoryTree() {
	if (this->_opened) {
		close();
	}
}

void MemoryOutHistoryTree::addInterval(AbstractInterval::SharedPtr interval) throw(TimeRangeEx) {
	
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

void MemoryOutHistoryTree::addSiblingNode(unsigned int index) {
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
		//this->serializeNode(this->_latest_branch[i]);
		
		// get parent node
		prev_node = dynamic_pointer_cast<CoreNode>(this->_latest_branch[i - 1]);
		assert(prev_node.get() != NULL);
		
		// is this a leaf node or a core node?
		new_node = this->initNewCoreNode(prev_node->getSequenceNumber(), split_time + 1);
		
		// link the new child to its parent
		prev_node->linkNewChild(new_node);
		
		this->_latest_branch[i] = new_node;
	}
	return;
}

void MemoryOutHistoryTree::initEmptyTree(void) {
	
	boost::unique_lock<boost::shared_mutex> bl(_latest_branch_mutex);
	// do init. stuff...
	this->_end = this->_config._treeStart;
	this->_node_count = 0;
	this->_latest_branch.clear();
	{
		boost::unique_lock<boost::shared_mutex> nl(_nodes_mutex);
		this->_nodes.clear();
	}
	
	// add a first (*leaf*) node
	CoreNode::SharedPtr n = this->initNewCoreNode(-1, this->_config._treeStart);
	_latest_branch.push_back(n);
}

void MemoryOutHistoryTree::addNewRootNode(void) {
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
		//this->serializeNode(this->_latest_branch[i]);
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
		new_node = this->initNewCoreNode(prev_node->getSequenceNumber(), split_time + 1);
		
		// link the new child to its parent
		prev_node->linkNewChild(new_node);
		
		this->_latest_branch.push_back(new_node);
	}
}


CoreNode::SharedPtr MemoryOutHistoryTree::initNewCoreNode(seq_number_t parent_seq, timestamp_t start) {
	// allocate new core node
	CoreNode::SharedPtr n(new CoreNode(this->_config, this->_node_count, parent_seq, start));
	
	{
		boost::unique_lock<boost::shared_mutex> l(_nodes_mutex);
		_nodes.push_back(n);
	}
	
	// new node count
	this->incNodeCount(start);
	
	return n;
}

LeafNode::SharedPtr MemoryOutHistoryTree::initNewLeafNode(seq_number_t parent_seq, timestamp_t start) {
	// allocate new leaf node
	LeafNode::SharedPtr n(new LeafNode(this->_config, this->_node_count, parent_seq, start));
	
	{
		boost::unique_lock<boost::shared_mutex> l(_nodes_mutex);
		_nodes.push_back(n);
	}
	
	// new node count
	this->incNodeCount(start);
	
	return n;
}


void MemoryOutHistoryTree::manageInsert(void){
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

void MemoryOutHistoryTree::startThread(void){
	
	_insertThread = boost::thread(boost::bind(&MemoryOutHistoryTree::manageInsert, this));
}

void MemoryOutHistoryTree::stopThread(void){
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
