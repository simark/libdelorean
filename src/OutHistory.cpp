/**
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
#include <fstream>
#include <ostream>
#include <cassert>
#include <tr1/memory>
#include <cstdlib>
 
#include <delorean/OutHistory.hpp>
#include <delorean/intervals/AbstractInterval.hpp>
#include <delorean/HistoryConfig.hpp>
#include <delorean/CoreNode.hpp>
#include <delorean/LeafNode.hpp>
#include <delorean/ex/IOEx.hpp>
#include <delorean/FixedConfig.hpp>
 
using namespace std;
using namespace std::tr1;
 
OutHistory::OutHistory()
: AbstractHistory() {
}

OutHistory::OutHistory(HistoryConfig config)
: AbstractHistory(config) {
}

/**
 * Opens a history tree file for writing only.
 * If the file already existed, it will be overwritten
 * 
 * @throw IOEx if file already open or general IO error
 */ 
void OutHistory::open() {
	// is this history tree already opened?
	if (this->_opened) {
		throw IOEx("This tree is already opened");
	}
	initEmptyTree();
	
	// open stream
	this->openStream();
	
	// update internal status
	this->_opened = true;
}

void OutHistory::close(timestamp_t end) {
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

void OutHistory::openStream(void) {
	if (this->_stream.is_open()) {
		throw IOEx("The stream is already open");
	}
	this->_stream.open(this->_config._stateFile.c_str(), fstream::out | fstream::binary | fstream::trunc);
	if (!this->_stream) {
		throw IOEx("Unable to open file");
	}
}

void OutHistory::closeStream(void) {
	if (this->_stream.is_open()) {
		this->_stream.close();
	}
}

OutHistory::~OutHistory() {
	if (this->_opened) {
		this->close();
	}
}

OutHistory& OutHistory::operator<<(AbstractInterval::SharedPtr interval) throw(TimeRangeEx) {
	this->addInterval(interval);
	
	return *this;
}

void OutHistory::addInterval(AbstractInterval::SharedPtr interval) throw(TimeRangeEx) {
	if (interval->getStart() < this->_config._treeStart) {
		throw TimeRangeEx("interval start time below tree start time");
	}
	this->tryInsertAtNode(interval, this->_latest_branch.size() - 1);
}

void OutHistory::tryInsertAtNode(AbstractInterval::SharedPtr interval, unsigned int index) {
	// target node
	
	AbstractNode::SharedPtr target_node = this->_latest_branch[index];
	
	/*cout << "trying " << *interval << " (" << interval->getTotalSize() << ")  fs " <<
		target_node->getFreeSpace() << "  seq " << target_node->getSequenceNumber() << endl;*/
	
	// is there enough room in prospective target node?
	if (interval->getTotalSize() > target_node->getFreeSpace()) {
		// nope: add to a new sibling instead
		this->addSiblingNode(index);
		this->tryInsertAtNode(interval, this->_latest_branch.size() - 1);
		
		return;
	}
	
	// make sure the interval time range fits this node
	if (interval->getStart() < target_node->getStart()) {
		// nope: check recursively in parents if it fits
		assert(index >= 1);
		this->tryInsertAtNode(interval, index - 1);
		
		return;
	}
	
	// everything seems okay here
	target_node->addInterval(interval);
	
	// update tree end time if needed
	if (interval->getEnd() > this->_end) {
		this->_end = interval->getEnd();
	}
}

void OutHistory::addSiblingNode(unsigned int index) {
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
	for (unsigned int i = index; i < this->_latest_branch.size(); ++i) {
		this->_latest_branch[i]->close(split_time);
		this->serializeNode(this->_latest_branch[i]);
		
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

void OutHistory::initEmptyTree(void) {
	// do init. stuff...
	this->_end = this->_config._treeStart;
	this->_node_count = 0;
	this->_latest_branch.clear();
	
	// add a first (*leaf*) node
	CoreNode::SharedPtr n = this->initNewCoreNode(-1, this->_config._treeStart);
	_latest_branch.push_back(n);
}

void OutHistory::addNewRootNode(void) {
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

void OutHistory::serializeHeader(void) {
	// since this is a private method, this should already be checked, but just in case...
	assert(this->_stream.is_open());
	fstream& f = this->_stream;
	f.seekp(0);
	
	// magic number
	int32_t magic_number = HF_MAGIC_NUMBER;
	f.write((char*) &magic_number, sizeof(int32_t));
	
	// versions
	int32_t major = HF_MAJOR;
	int32_t minor = HF_MINOR;
	f.write((char*) &major, sizeof(int32_t));
	f.write((char*) &minor, sizeof(int32_t));
	
	// block size
	int32_t bs = (int32_t) this->_config._blockSize;
	f.write((char*) &bs, sizeof(int32_t));
	
	// maximum children/node
	int32_t mc = (int32_t) this->_config._maxChildren;
	f.write((char*) &mc, sizeof(int32_t));
	
	// node count
	int32_t node_count = (int32_t) this->_node_count;
	f.write((char*) &node_count, sizeof(int32_t));
	
	// root node sequence number
	seq_number_t root_seq = this->_latest_branch[0]->getSequenceNumber();
	f.write((char*) &root_seq, sizeof(seq_number_t));
}

void OutHistory::serializeNode(AbstractNode::SharedPtr node) {
	// seek to correct position
	this->_stream.seekp(this->filePosFromSeq(node->getSequenceNumber()));
	
	// serialize node as is
	node->serialize(this->_stream);
}

void OutHistory::incNodeCount(timestamp_t new_start) {
	// increment our node count since we now have one more
	++this->_node_count;
	
	// update tree's end time if needed
	if (new_start >= this->_end) {
		this->_end = new_start + 1;
	}
}

CoreNode::SharedPtr OutHistory::initNewCoreNode(seq_number_t parent_seq, timestamp_t start) {
	// allocate new core node
	CoreNode::SharedPtr n(new CoreNode(this->_config, this->_node_count, parent_seq, start));
	
	// new node count
	this->incNodeCount(start);
	
	return n;
}

LeafNode::SharedPtr OutHistory::initNewLeafNode(seq_number_t parent_seq, timestamp_t start) {
	// allocate new leaf node
	LeafNode::SharedPtr n(new LeafNode(this->_config, this->_node_count, parent_seq, start));
	
	// new node count
	this->incNodeCount(start);
	
	return n;
}