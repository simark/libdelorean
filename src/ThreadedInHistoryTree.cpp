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
 
#include <rbrntrvll/ThreadedInHistoryTree.hpp>
#include <rbrntrvll/InHistoryTree.hpp>
#include <rbrntrvll/intervals/AbstractInterval.hpp>
#include <rbrntrvll/HistoryTreeConfig.hpp>
#include <rbrntrvll/CoreNode.hpp>
#include <rbrntrvll/LeafNode.hpp>
#include <rbrntrvll/ex/IOEx.hpp>
#include <rbrntrvll/ex/UnknownNodeTypeEx.hpp>
#include <rbrntrvll/fixed_config.h>
#include <sstream>

#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/shared_mutex.hpp>


using namespace std;
using namespace std::tr1;

ThreadedInHistoryTree::ThreadedInHistoryTree()
: AbstractHistoryTree() , InHistoryTree(), AbstractThreadedHistoryTree(){
}

ThreadedInHistoryTree::ThreadedInHistoryTree(HistoryTreeConfig config)
: AbstractHistoryTree(config), InHistoryTree(config), AbstractThreadedHistoryTree(config) {
}

vector<AbstractInterval::SharedPtr> ThreadedInHistoryTree::query(timestamp_t timestamp) const {
	if ( !checkValidTime(timestamp) ) {
		throw TimeRangeEx("Query timestamp outside of bounds");
	}
	
	// We start by reading the information in the root node
	AbstractNode::SharedPtr currentNode;
	{
		boost::shared_lock<boost::shared_mutex> l(_latest_branch_mutex);
		currentNode = _latest_branch[0];
	}
	vector<AbstractInterval::SharedPtr> relevantIntervals;
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

AbstractInterval::SharedPtr ThreadedInHistoryTree::query(timestamp_t timestamp, attribute_t key) const {
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

std::multimap<attribute_t, AbstractInterval::SharedPtr> ThreadedInHistoryTree::sparseQuery(timestamp_t timestamp) const {
	if ( !checkValidTime(timestamp) ) {
		throw TimeRangeEx("Query timestamp outside of bounds");
	}
	
	// We start by reading the information in the root node
	AbstractNode::SharedPtr currentNode;
	{
		boost::shared_lock<boost::shared_mutex> l(_latest_branch_mutex);
		currentNode = _latest_branch[0];
	}
	multimap<attribute_t, AbstractInterval::SharedPtr> relevantIntervals;
	currentNode->writeInfoFromNode(relevantIntervals, timestamp);
	
	
	// Then we follow the branch down in the relevant children
	// Stop at leaf nodes or if a core node has no children
	while ( nodeHasChildren(currentNode) ) {
		CoreNode::SharedPtr coreNode = dynamic_pointer_cast<CoreNode>(currentNode);
		assert(coreNode != NULL);
		currentNode = selectNextChild(coreNode, timestamp);
		currentNode->writeInfoFromNode(relevantIntervals, timestamp);
	}
	
	// The relevantIntervals should now be filled with everything needed
	return relevantIntervals;	
}

AbstractNode::SharedPtr ThreadedInHistoryTree::createNodeFromStream() const {
	fstream& f = this->_stream;
	boost::unique_lock<boost::recursive_mutex> l(_stream_mutex);
	unsigned int init_pos = f.tellg();
	
	// node to return
	AbstractNode::SharedPtr n;
	
	// type
	node_type_t nt;
	f.read((char*) &nt, sizeof(node_type_t));
	
	// create node according to type
	switch (nt) {
		case NT_CORE:
		n.reset(new CoreNode(this->_config));
		break;
		
		case NT_LEAF:
		n.reset(new LeafNode(this->_config));
		break;
		
		default:
		throw(UnknownNodeTypeEx(nt));
		break;
	}
	
	// unserialize the node
	f.seekg(init_pos);
	n->unserialize(f, this->_ic);
	
	return n;
}

AbstractNode::SharedPtr ThreadedInHistoryTree::createNodeFromSeq(seq_number_t seq) const {
	// make sure everything is okay
	assert((unsigned int) seq < this->_node_count);
		
	// compute where the node begins in file
	unsigned int offset = this->getHeaderSize() + seq * this->_config._blockSize;
	
	// seek there
	boost::unique_lock<boost::recursive_mutex> l(_stream_mutex);
	this->_stream.seekg(offset, ios::beg);
	
	// get the node
	return this->createNodeFromStream();
}

AbstractNode::SharedPtr ThreadedInHistoryTree::fetchNodeFromLatestBranch(seq_number_t seq) const {

	std::vector<AbstractNode::SharedPtr>::const_iterator it;
	
	boost::shared_lock<boost::shared_mutex> l(_latest_branch_mutex);
	
	for (it = _latest_branch.begin(); it != _latest_branch.end(); it++) {
		if ((*it)->getSequenceNumber() == seq) {
			return *it;
		}
	}
	return AbstractNode::SharedPtr();
}
