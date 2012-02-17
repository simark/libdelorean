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
#include "HistoryTreeNode.hpp"
#include "HistoryTree.hpp"
#include "IntInterval.hpp"
#include "basic_types.h"

#include <algorithm>
#include <assert.h>
#include <ostream>
#include <cstring>

using namespace std;
using namespace std::tr1;

bool orderIntervals (shared_ptr<Interval> i, shared_ptr<Interval> j) { return (*i<*j); }

HistoryTreeNode::HistoryTreeNode()
:_nbChildren(0)
{
}

HistoryTreeNode::HistoryTreeNode(HistoryTree& tree, int seqNumber, int parentSeqNumber, timestamp_t start)
:_ownerTree(&tree), _nodeStart(start), _sequenceNumber(seqNumber), _parentSequenceNumber(parentSeqNumber), _nbChildren(0)
{
	_variableSectionOffset = _ownerTree->getConfig()._blockSize;
	_isDone = false;
	_intervals = std::vector<shared_ptr<Interval> >();
}

HistoryTreeNode::~HistoryTreeNode()
{
}

/**
 * Reader factory constructor. Build a Node object (of the right type)
 * by reading a block in the file.
 *  
 * @param tree Reference to the HT which will own this node
 * @param fc FileChannel to the history file, ALREADY SEEKED at the start of the node.
 * @throw IOException 
 */
HistoryTreeNode HistoryTreeNode::readNode(const HistoryTree& tree)
{
	//FIXME Do the complex parsing and IO here
	return HistoryTreeNode();
}

/**
 * The method to fill up the stateInfo (passed on from the Current State Tree when
 * it does a query on the SHT). We'll replace the data in that vector with whatever
 * relevant we can find from this node
 * 
 * @param stateInfo The same stateInfo that comes from SHT's doQuery()
 * @param t The timestamp for which the query is for. Only return intervals that intersect t.
 * @throw TimeRangeException 
 */
void HistoryTreeNode::writeInfoFromNode(vector<shared_ptr<Interval> >& intervals, timestamp_t timestamp) const
{
	int startIndex;

	if ( _intervals.size() == 0 ) { return; }
	startIndex = getStartIndexFor(timestamp);

	for ( unsigned int i = startIndex; i < _intervals.size(); i++ ) {
		/* Now we only have to compare the Start times, since we know
		 * the End times necessarily fit */
		if ( _intervals[i]->getStart() <= timestamp ) {
			intervals[_intervals[i]->getAttribute()] = _intervals[i];
		}
	}
	return;
}

/**
 * Add an interval to this node
 * @param newInterval
 */
void HistoryTreeNode::addInterval(const Interval& newInterval)
{
	/* Just in case, but should be checked before even calling this function */
	assert( newInterval.getTotalSize() <= getFreeSpace() );
	
	/* We need to clone the interval, to guarantee ownership */
	_intervals.push_back( shared_ptr<Interval>(newInterval.clone()) );
	
	/* Update the in-node offset "pointer" */
	_variableSectionOffset -= ( newInterval.getVariableValueSize() );	
}

/**
 * We've received word from the containerTree that newest nodes now exist to
 * our right. (Puts isDone = true and sets the endtime)
 * 
 * @param endtime The nodeEnd time that the node will have
 * @throw TimeRangeException 
 */
void HistoryTreeNode::closeThisNode(timestamp_t endtime)
{
	assert ( endtime >= _nodeStart );
//	/* This also breaks often too */
//	if ( endtime.getValue() <= this.nodeStart.getValue() ) {
//		_ownerTree->debugPrintFullTree(new PrintWriter(System.out, true), null, false);
//		assert ( false );
//	}
	
	if ( _intervals.size() > 0 ) {
		/* Sort the intervals by ascending order of their end time.
		 * This speeds up lookups a bit */
		 /* Lambda functions will not compile for some reason, using a normal function instead*/
		std::sort(_intervals.begin(), _intervals.end(), 		
		//[](std::tr1::shared_ptr<Interval> a, std::tr1::shared_ptr<Interval> b) -> bool { return *a < *b; });
		orderIntervals);
		
		/* Make sure there are no intervals in this node with their
		 * EndTime > the one requested. Only need to check the last one
		 * since they are now sorted */
		assert ( endtime >= _intervals[_intervals.size()-1]->getEnd() );
	}
	
	_isDone = true;
	_nodeEnd = endtime;
	return;	
}

/**
 * Tell this node that it has a new child (Congrats!)
 * 
 * @param childNode The SHTNode object of the new child
 */
void HistoryTreeNode::linkNewChild(const HistoryTreeNode& childNode)
{
	assert( _nbChildren < _ownerTree->getConfig()._maxChildren );
	
	_children[_nbChildren] = childNode.getSequenceNumber();
	_childStart[_nbChildren] = childNode.getNodeStart();
	_nbChildren++;	
}

/**
 * Get a single Interval from the information in this node
 * If the key/timestamp pair cannot be found, we return a null pointer.
 * 
 * @param key
 * @param t
 * @return The Interval containing the information we want, or null if it wasn't found
 * @throw TimeRangeEx
 */
std::tr1::shared_ptr<Interval> HistoryTreeNode::getRelevantInterval(timestamp_t timestamp, attribute_t key) const
{
	assert ( _isDone );
	int startIndex;
	
	if ( _intervals.size() == 0 ) { return shared_ptr<Interval>(); }
	
	startIndex = getStartIndexFor(timestamp);
	
	for ( unsigned int i = startIndex; i < _intervals.size(); i++ ) {
		if ( _intervals[i]->getAttribute() == key ) {
			if ( _intervals[i]->getStart() <= timestamp ) {
				return _intervals[i];
			}
		}
	}
	/* We didn't find the relevant information in this node */
	return shared_ptr<Interval>();
}

/**
 * Finds the minimum index in the intervals container to start the search.
 * Since the intervals are ordered by ending time, it is possible to skip
 * the first ones and use an efficient search algorithm to find the first
 * interval that could possibly hold a given timestamp
 *  
 * @param timestamp
 * @return the index of the first interval in _intervals that could hold this timestamp
 */
int HistoryTreeNode::getStartIndexFor(timestamp_t timestamp) const
{
	//FIXME test this thoroughly
	
	// This should prevent the rare case when timestamp = 0, and timestamp-1 could behave erratically
	if(timestamp == _nodeStart) { return 0;}
	
	shared_ptr<Interval> dummyInterval(new IntInterval(0, timestamp-1, 0, 0));
	vector<shared_ptr<Interval> >::const_iterator it;
	
	
	it = lower_bound(_intervals.begin(), _intervals.end(), dummyInterval, 
	//[](std::tr1::shared_ptr<Interval> a, std::tr1::shared_ptr<Interval> b) -> bool { return *a < *b; });
	orderIntervals);
	
	return it - _intervals.begin();
}

/**
 * Returns the free space in the node, which is simply put,
 * the stringSectionOffset - dataSectionOffset
 */
unsigned int HistoryTreeNode::getFreeSpace()
{
	return _variableSectionOffset - getDataSectionEndOffset();
}

int HistoryTreeNode::getTotalHeaderSize() const
{
	int headerSize = 0;

#if 0
	// FIXME: not so sure about describing the whole header here
	// FIXME: this only applies to core nodes
	headerSize +=	  4	/* 1x int (extension node) */
			+ 4	/* 1x int (nbChildren) */
			+ 4 * _ownerTree->getConfig()._maxChildren	/* MAX_NB * int ('children' table) */
			+ 8 * _ownerTree->getConfig()._maxChildren	/* MAX_NB * Timevalue ('childStart' table) */
			+ 1 	/* byte (type) */
			+ 16	/* 2x long (start time, end time) */
			+ 16	/* 4x int (seq number, parent seq number, intervalcount, strings section pos.) */
			+ 1;	/* byte (done or not) */
#endif

	return 34;
}

/**
 * @return The offset, within the node, where the Data (intervals) section ends
 */
int HistoryTreeNode::getDataSectionEndOffset() const
{
	return getTotalHeaderSize() + Interval::getHeaderSize() * _intervals.size();
}

void HistoryTreeNode::serialize(ostream& os) {
	// allocate some byte buffer
	// TODO: private buffer to avoid new/delete for each block write?
	unsigned int bufsz = _ownerTree->getConfig()._blockSize;
	uint8_t* buf = new uint8_t [bufsz];
	
	// serialize to buffer
	this->serialize(buf);
	
	// write to output
	os.write((char*) buf, bufsz);
	
	// free buffer
	delete [] buf;
}

void HistoryTreeNode::serialize(uint8_t* buf) {
	// pointer backup
	uint8_t* bkbuf = buf;
	
	// prepare common header fields
	int32_t interval_count = (int32_t) this->_intervals.size();
	int32_t var_sect_offset = (int32_t) _variableSectionOffset;
	uint8_t isDone = this->_isDone ? 1 : 0;
	
	// write block common header
	memcpy(buf, &this->_nodeType, sizeof(uint8_t));
	buf += sizeof(uint8_t);
	memcpy(buf, &this->_nodeStart, sizeof(timestamp_t));
	buf += sizeof(timestamp_t);
	memcpy(buf, &this->_nodeEnd, sizeof(timestamp_t));
	buf += sizeof(timestamp_t);
	memcpy(buf, &this->_sequenceNumber, sizeof(seq_number_t));
	buf += sizeof(seq_number_t);
	memcpy(buf, &this->_parentSequenceNumber, sizeof(seq_number_t));
	buf += sizeof(seq_number_t);
	memcpy(buf, &interval_count, sizeof(int32_t));
	buf += sizeof(int32_t);
	memcpy(buf, &var_sect_offset, sizeof(int32_t));
	buf += sizeof(int32_t);
	memcpy(buf, &isDone, sizeof(uint8_t));
	buf += sizeof(uint8_t);
	
	// write node's specific header
	unsigned int sheadsz = this->writeHeader(buf);
	buf += sheadsz;
	
	// write intervals (OO fashion)
	vector< shared_ptr<Interval> >::iterator it;
	uint8_t* var_addr = bkbuf + _ownerTree->getConfig()._blockSize;
	for (it = this->_intervals.begin(); it != this->_intervals.end(); ++it) {	
		shared_ptr<Interval> interval = *it;
		// get interval variable value size
		unsigned int var_size = interval->getVariableValueSize();
		var_addr -= var_size;
		unsigned int offset_ptr = ((unsigned int) var_addr - (unsigned int) bkbuf);
		
		// serialize interval
		interval->serialize(var_addr, buf);
		
		// overwrite header's value with variable value "pointer" if variable size isn't 0
		if (var_size != 0) {
			memcpy(buf + interval->getHeaderSize() - sizeof(uint32_t), &offset_ptr, sizeof(uint32_t));
		}
		buf += interval->getHeaderSize();
	}
}
