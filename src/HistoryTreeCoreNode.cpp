/**
 * Copyright (c) 2012 Philippe Proulx <philippe.proulx@polymtl.ca>
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
#include <algorithm>
#include <cassert>
#include <ostream>
#include <cstring>
#include <sstream>

#include "HistoryTreeConfig.hpp"
#include "HistoryTreeCoreNode.hpp"
#include "HistoryTreeNode.hpp"
#include "basic_types.h"
#include "fixed_config.h"

using namespace std;
using namespace std::tr1;

HistoryTreeCoreNode::HistoryTreeCoreNode(HistoryTreeConfig config, seq_number_t seqNumber,
seq_number_t parentSeqNumber, timestamp_t start)
: HistoryTreeNode(config, seqNumber, parentSeqNumber, start, NT_CORE), _nbChildren(0) {
	this->initChildren();
}

HistoryTreeCoreNode::HistoryTreeCoreNode(HistoryTreeConfig config)
: HistoryTreeNode(config) {
	this->initChildren();
}

HistoryTreeCoreNode::~HistoryTreeCoreNode() {
	this->finiChildren();
}

void HistoryTreeCoreNode::initChildren(void) {
	// initialize children data
	this->_children = new seq_number_t [this->_config._maxChildren];
	this->_childStart = new timestamp_t [this->_config._maxChildren];
}

void HistoryTreeCoreNode::finiChildren(void) {	
	// free children data
	delete [] this->_children;
	delete [] this->_childStart;
}

unsigned int HistoryTreeCoreNode::getSpecificHeaderSize(void) const
{
	const unsigned int mc = this->_config._maxChildren;
	
	return (
		sizeof(seq_number_t) +		// extended? (not used)
		sizeof(uint32_t) +		// children count
		sizeof(seq_number_t) * mc +	// children sequence numbers
		sizeof(timestamp_t) * mc	// children start time stamps
	);
}

void HistoryTreeCoreNode::linkNewChild(HistoryTreeNodeSharedPtr childNode)
{
	assert (_nbChildren < _config._maxChildren);

	_children[_nbChildren] = childNode->getSequenceNumber();
	_childStart[_nbChildren] = childNode->getStart();
	_nbChildren++;
}

/**
 * Returns the child's sequence number for a given timestamp
 * Only one child can possibly hold this timestamp (no overlapping)
 * 
 * @param timestamp
 * @return child sequence number or node's sequence number if no valid child
 */
seq_number_t HistoryTreeCoreNode::getChildAtTimestamp(timestamp_t timestamp) const
{
	seq_number_t potentialNextSeqNb = getSequenceNumber();
	
	for ( unsigned int i = 0; i < getNbChildren(); i++ ) {
		if ( timestamp >= getChildStart(i) ) {
			potentialNextSeqNb = getChild(i);
		} else {
			break;
		}
	}
	return potentialNextSeqNb;
}

void HistoryTreeCoreNode::serializeSpecificHeader(uint8_t* buf) const
{	
	// buffer pointer backup
	uint8_t* bkbuf = buf;
	
	// extended sequence number (not used)
	seq_number_t extended = -1;
	memcpy(buf, &extended, sizeof(seq_number_t));
	buf += sizeof(seq_number_t);
	
	// children count
	int32_t nbc = (int32_t) this->_nbChildren;
	memcpy(buf, &nbc, sizeof(int32_t));
	buf += sizeof(int32_t);
	
	// children sequence number
	const unsigned int mc = this->_config._maxChildren;
	memcpy(buf, this->_children, sizeof(seq_number_t) * mc);
	buf += (sizeof(seq_number_t) * mc);
	
	// children start time stamps
	memcpy(buf, this->_childStart, sizeof(timestamp_t) * mc);
}

void HistoryTreeCoreNode::unserializeSpecificHeader(std::istream& is) {
	// extended? we don't care
	is.seekg(sizeof(int32_t), ios::cur);
	
	// children count
	is.read((char*) &this->_nbChildren, sizeof(int32_t));
	
	// get children
	is.read((char*) this->_children, sizeof(seq_number_t) * this->_config._maxChildren);
	is.read((char*) this->_childStart, sizeof(timestamp_t) * this->_config._maxChildren);
}

std::string HistoryTreeCoreNode::getInfos(void) const {
	ostringstream oss;
	
	oss << ", " << this->_nbChildren << " children";
	for (unsigned int i = 0; i < this->_nbChildren; ++i) {
		oss << endl << "  + {" << this->_children[i] << "} " <<
			"[" << this->_childStart[i] << "]";
	}
	
	return oss.str();
}
