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
 
#include "InHistoryTree.hpp"
#include "Interval.hpp"
#include "HistoryTreeConfig.hpp"
#include "HistoryTreeCoreNode.hpp"
#include "HistoryTreeLeafNode.hpp"
#include "ex/IOEx.hpp"
#include "ex/UnknownNodeTypeEx.hpp"
#include "fixed_config.h"

using namespace std;
using namespace std::tr1;

InHistoryTree::InHistoryTree()
: AbstractHistoryTree() {
}

InHistoryTree::InHistoryTree(HistoryTreeConfig config)
: AbstractHistoryTree(config) {
}

void InHistoryTree::open(void) {
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
	this->unserializeHeader();
	
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
 * From an existing tree on the disk, rebuild the latest branch.
 * 
 */ 
void InHistoryTree::buildLatestBranch(void) {
	assert(this->_node_count > 0);
	fstream& f = this->_stream;
	
	this->_latest_branch.clear();
	
	//Read root
	HistoryTreeNodeSharedPtr node = createNodeFromSeq(this->_root_seq);
	HistoryTreeCoreNodeSharedPtr coreNode = dynamic_pointer_cast<HistoryTreeCoreNode>(node);
	_latest_branch.push_back(node);
	
	//Follow the latest branch down
	while(coreNode != NULL && coreNode->getNbChildren() > 0) {
		unsigned int nbChildren = coreNode->getNbChildren();
		seq_number_t nextSeq = coreNode->getChild(nbChildren-1);
		node = createNodeFromSeq(nextSeq);
		this->_latest_branch.push_back(node);
		coreNode = dynamic_pointer_cast<HistoryTreeCoreNode>(node);
	}
	return;
}

void InHistoryTree::unserializeHeader(void) {
	fstream& f = this->_stream;
	// goto beginning
	f.seekg(0);
	
	// verify that this is an history tree file
	int32_t mn;
	f.read((char*) &mn, sizeof(int32_t));
	if (mn != HF_MAGIC_NUMBER) {
		throw IOEx("Invalid file : incorrect format");
	}
	
	// file version
	int32_t major, minor;
	f.read((char*) &major, sizeof(int32_t));
	f.read((char*) &minor, sizeof(int32_t));
	if (major != HF_MAJOR || minor != HF_MINOR) {
		throw IOEx("Invalid file : unsupported version");
	}
	
	// block size
	f.read((char*) &this->_config._blockSize, sizeof(int32_t));
	
	// max. children
	f.read((char*) &this->_config._maxChildren, sizeof(int32_t));
	
	// node count
	f.read((char*) &this->_node_count, sizeof(int32_t));
	
	// root sequence number
	f.read((char*) &this->_root_seq, sizeof(int32_t));
}

void InHistoryTree::close(timestamp_t end) {	
	// is this history tree at least opened?
	if (!this->_opened) {
		throw IOEx("This tree was not open");
	}
	
	// close stream
	this->_stream.close();
	
	// update internal status
	this->_opened = false;
}

InHistoryTree::~InHistoryTree() {
	if (this->_opened) {
		this->close();
	}
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
HistoryTreeNodeSharedPtr InHistoryTree::selectNextChild(HistoryTreeCoreNodeSharedPtr currentNode, timestamp_t timestamp) const {
	assert ( currentNode->getNbChildren() > 0 );
	int potentialNextSeqNb = currentNode->getChildAtTimestamp(timestamp);
	
	/* Once we exit this loop, we should have found a children to follow.
	 * If we didn't, there's a problem. */
	assert ( potentialNextSeqNb != currentNode->getSequenceNumber() );
	
	/* Since this code path is quite performance-critical, avoid iterating
	 * through the whole latestBranch array if we know for sure the next
	 * node has to be on disk */
	if ( currentNode->isDone() ) {
		return createNodeFromSeq(potentialNextSeqNb);
	} else {
		return fetchNodeFromLatestBranch(potentialNextSeqNb);
	}
}

vector<IntervalSharedPtr> InHistoryTree::query(timestamp_t timestamp) const {
	if ( !checkValidTime(timestamp) ) {
		throw TimeRangeEx("Query timestamp outside of bounds");
	}
	
	// We start by reading the information in the root node
	HistoryTreeNodeSharedPtr currentNode = _latest_branch[0];
	vector<IntervalSharedPtr> relevantIntervals;
	currentNode->writeInfoFromNode(relevantIntervals, timestamp);
	
	
	// Then we follow the branch down in the relevant children
	// Stop at leaf nodes or if a core node has no children
	while ( nodeHasChildren(currentNode) ) {
		HistoryTreeCoreNodeSharedPtr coreNode = dynamic_pointer_cast<HistoryTreeCoreNode>(currentNode);
		currentNode = selectNextChild(coreNode, timestamp);
		currentNode->writeInfoFromNode(relevantIntervals, timestamp);
	}
	
	// The relevantIntervals should now be filled with everything needed
	return relevantIntervals;	
}

IntervalSharedPtr InHistoryTree::query(timestamp_t timestamp, attribute_t key) const {
	if ( !checkValidTime(timestamp) ) {
		throw TimeRangeEx("Query timestamp outside of bounds");
	}
	
	HistoryTreeNodeSharedPtr currentNode = _latest_branch[0];
	IntervalSharedPtr interval = currentNode->getRelevantInterval(timestamp, key);
	
	// Follow the branch down until we find the required interval or there are no children left
	// Stop at leaf nodes or if a core node has no children
	while ( interval == NULL && nodeHasChildren(currentNode) ) {
		HistoryTreeCoreNodeSharedPtr coreNode = dynamic_pointer_cast<HistoryTreeCoreNode>(currentNode);
		currentNode = selectNextChild(coreNode, timestamp);
		interval = currentNode->getRelevantInterval(timestamp, key);
	}
	/* Since we should now have intervals at every attribute/timestamp
	 * combination, it should NOT be null here. */
	assert (interval != NULL);
	
	return interval;
}

HistoryTreeNodeSharedPtr InHistoryTree::createNodeFromStream() const {
	fstream& f = this->_stream;
	unsigned int init_pos = f.tellg();
	
	// node to return
	HistoryTreeNodeSharedPtr n;
	
	// type
	node_type_t nt;
	f.read((char*) &nt, sizeof(node_type_t));
	
	// create node according to type
	switch (nt) {
		case NT_CORE:
		n.reset(new HistoryTreeCoreNode(this->_config));
		break;
		
		case NT_LEAF:
		n.reset(new HistoryTreeLeafNode(this->_config));
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

HistoryTreeNodeSharedPtr InHistoryTree::createNodeFromSeq(seq_number_t seq) const {
	// make sure everything is okay
	assert((unsigned int) seq < this->_node_count);
	
	// compute where the node begins in file
	unsigned int offset = this->getHeaderSize() + seq * this->_config._blockSize;
	
	// seek there
	this->_stream.seekg(offset, ios::beg);
	
	// get the node
	return this->createNodeFromStream();
}

HistoryTreeNodeSharedPtr InHistoryTree::fetchNodeFromLatestBranch(seq_number_t seq) const {

	std::vector<HistoryTreeNodeSharedPtr>::const_iterator it;
	
	for (it = _latest_branch.begin(); it != _latest_branch.end(); it++) {
		if ((*it)->getSequenceNumber() == seq) {
			return *it;
		}
	}
	return HistoryTreeNodeSharedPtr();
}

void InHistoryTree::test(void) {
	for (unsigned int i = 0; i < this->_node_count; ++i) {
		HistoryTreeNodeSharedPtr node(this->createNodeFromSeq(i));
		cout << *node << endl;
	}
}
