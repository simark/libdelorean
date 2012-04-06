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
 
#include "MemoryInHistoryTree.hpp"
#include "InHistoryTree.hpp"
#include "intervals/AbstractInterval.hpp"
#include "HistoryTreeConfig.hpp"
#include "CoreNode.hpp"
#include "LeafNode.hpp"
#include "ex/IOEx.hpp"
#include "ex/UnknownNodeTypeEx.hpp"
#include "fixed_config.h"
#include <sstream>

#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/shared_mutex.hpp>


using namespace std;
using namespace std::tr1;

MemoryInHistoryTree::MemoryInHistoryTree()
: AbstractHistoryTree() , InHistoryTree(), AbstractMemoryHistoryTree(){
}

MemoryInHistoryTree::MemoryInHistoryTree(HistoryTreeConfig config)
: AbstractHistoryTree(config), InHistoryTree(config), AbstractMemoryHistoryTree(config) {
}

/**
 * Opens a history tree file for reading only.
 * The file must exist and be of the correct format.
 * 
 * @throw IOEx if no file, incorrect format, or already open
 * 
 */
void MemoryInHistoryTree::open() {
	// is this history tree already opened?
	if (this->_opened) {
		throw IOEx("This tree is already opened");
	}
	
	// open stream
	this->_stream.open(this->_config._stateFile.c_str(), fstream::in | fstream::binary);
	
	// check for open errors
	if (!this->_stream) {
		throw IOEx("Unable to open file");
	}
	
	// unserialize tree header
	try{
		this->unserializeHeader();
	}catch(InvalidFormatEx& ex){
		this->_stream.close();
		throw;
	}catch(IOEx& ex){
		this->_stream.close();
		throw;
	}
	
	// load everything in memory
	this->loadNodes();
	
	// store latest branch in memory
	this->buildLatestBranch();
	
	// The user could not possibly know the start and end times of the tree
	// Set it to the correct value using the root node
	_config._treeStart = _latest_branch[0]->getStart();
	_end = _latest_branch[0]->getEnd();
	
	// We no longer need the file for the rest of the process
	this->_stream.close();
	
	// update internal status
	this->_opened = true;
}

void MemoryInHistoryTree::close(timestamp_t end) {	
	// is this history tree at least opened?
	if (!this->_opened) {
		throw IOEx("This tree was not open");
	}
	
	// update internal status
	this->_opened = false;
}

/**
 * Inner method to select the next child of the current node intersecting
 * the given timestamp. Useful for moving down the tree following one
 * branch.
 * 
 * @param currentNode
 * @param t
 * @return The child node intersecting t
 */
AbstractNode::SharedPtr MemoryInHistoryTree::selectNextChild(CoreNode::SharedPtr currentNode, timestamp_t timestamp) const {
	assert ( currentNode->getNbChildren() > 0 );
	int potentialNextSeqNb = currentNode->getChildAtTimestamp(timestamp);
	
	/* Once we exit this loop, we should have found a children to follow.
	 * If we didn't, there's a problem. */
	assert ( potentialNextSeqNb != currentNode->getSequenceNumber() );
	
	return createNodeFromSeq(potentialNextSeqNb);
}

vector<AbstractInterval::SharedPtr> MemoryInHistoryTree::query(timestamp_t timestamp) const {
	if ( !checkValidTime(timestamp) ) {
		throw TimeRangeEx("Query timestamp outside of bounds");
	}
	
	// We start by reading the information in the root node
	AbstractNode::SharedPtr currentNode;
	{
		boost::shared_lock<boost::shared_mutex> l(_latest_branch_mutex);
		currentNode = _latest_branch[0];
	}
	vector<AbstractInterval::SharedPtr> relevantIntervals(1);
	currentNode->writeInfoFromNode(relevantIntervals, timestamp);
	
	
	// Then we follow the branch down in the relevant children
	// Stop at leaf nodes or if a core node has no children
	while ( nodeHasChildren(currentNode) ) {
		CoreNode::SharedPtr coreNode = dynamic_pointer_cast<CoreNode>(currentNode);
		currentNode = selectNextChild(coreNode, timestamp);
		currentNode->writeInfoFromNode(relevantIntervals, timestamp);
	}
	
	// The relevantIntervals should now be filled with everything needed
	return relevantIntervals;	
}

AbstractInterval::SharedPtr MemoryInHistoryTree::query(timestamp_t timestamp, attribute_t key) const {
	if ( !checkValidTime(timestamp) ) {
		throw TimeRangeEx("Query timestamp outside of bounds");
	}
	
	AbstractNode::SharedPtr currentNode;
	{
		boost::shared_lock<boost::shared_mutex> l(_latest_branch_mutex);
		currentNode = _latest_branch[0];
	}
	AbstractInterval::SharedPtr interval = currentNode->getRelevantInterval(timestamp, key);
	
	// Follow the branch down until we find the required interval or there are no children left
	// Stop at leaf nodes or if a core node has no children
	while ( interval == NULL && nodeHasChildren(currentNode) ) {
		CoreNode::SharedPtr coreNode = dynamic_pointer_cast<CoreNode>(currentNode);
		currentNode = selectNextChild(coreNode, timestamp);
		interval = currentNode->getRelevantInterval(timestamp, key);
	}
	/* Since we should now have intervals at every attribute/timestamp
	 * combination, it should NOT be null here. */
	assert (interval != NULL);
	
	return interval;
}

/**
 * It makes no sense to call this method in a memory history tree
 * @return null pointer
 */ 
AbstractNode::SharedPtr MemoryInHistoryTree::createNodeFromStream() const {
	
	return InHistoryTree::createNodeFromStream();
}

AbstractNode::SharedPtr MemoryInHistoryTree::createNodeFromSeq(seq_number_t seq) const {
	
	boost::shared_lock<boost::shared_mutex> l(_nodes_mutex);
	
	// make sure everything is okay
	assert((unsigned int) seq < this->_node_count);
	
	// get the node
	return this->_nodes[seq];
}

AbstractNode::SharedPtr MemoryInHistoryTree::fetchNodeFromLatestBranch(seq_number_t seq) const {

	std::vector<AbstractNode::SharedPtr>::const_iterator it;
	
	boost::shared_lock<boost::shared_mutex> l(_latest_branch_mutex);
	
	for (it = _latest_branch.begin(); it != _latest_branch.end(); it++) {
		if ((*it)->getSequenceNumber() == seq) {
			return *it;
		}
	}
	return AbstractNode::SharedPtr();
}

void MemoryInHistoryTree::loadNodes() {
	
	//Not required to acquire mutex since the tree is not yet opened
	
	_nodes.clear();
	// compute where the nodes begin in file
	unsigned int offset = this->getHeaderSize();
	
	for(unsigned int i = 0; i < this->_node_count; ++i){
		 
		// seek there
		this->_stream.seekg(offset, ios::beg);
		// get the node
		AbstractNode::SharedPtr node = this->createNodeFromStream();
		_nodes.push_back(node);
		 
		 offset += this->_config._blockSize;
	}
}
