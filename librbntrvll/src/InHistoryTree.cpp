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
	
	// update internal status
	this->_opened = true;
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

HistoryTreeNodeSharedPtr InHistoryTree::createNodeFromStream() {
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
		// throw something (unknown node type)
		break;
	}
	
	// unserialize the node
	f.seekg(init_pos);
	n->unserialize(f, this->_ic);
	
	return n;
}

HistoryTreeNodeSharedPtr InHistoryTree::createNodeFromSeq(seq_number_t seq) {
	// make sure everything is okay
	assert((unsigned int) seq < this->_node_count);
	
	// compute where the node begins in file
	unsigned int offset = this->getHeaderSize() + seq * this->_config._blockSize;
	
	// seek there
	this->_stream.seekg(offset, ios::beg);
	
	// get the node
	return this->createNodeFromStream();
}

void InHistoryTree::test(void) {
	for (unsigned int i = 0; i < this->_node_count; ++i) {
		HistoryTreeNodeSharedPtr node(this->createNodeFromSeq(i));
		cout << *node << endl;
	}
}
