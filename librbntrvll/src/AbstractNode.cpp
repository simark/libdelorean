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
#include <assert.h>
#include <ostream>
#include <cstring>
#include <sstream>

#include "intervals/NullInterval.hpp"
#include "IntervalCreator.hpp"
#include "AbstractNode.hpp"
#include "basic_types.h"
#include "ex/NodeFullEx.hpp"

using namespace std;
using namespace std::tr1;

const unsigned int AbstractNode::COMMON_HEADER_SIZE = 34;

bool orderIntervals (AbstractInterval::SharedPtr i, AbstractInterval::SharedPtr j) { return (*i<*j); }

AbstractNode::AbstractNode(HistoryTreeConfig config, seq_number_t seqNumber,
seq_number_t parentSeqNumber, timestamp_t start, node_type_t type)
: _config(config), _nodeStart(start), _sequenceNumber(seqNumber), _parentSequenceNumber(parentSeqNumber), _type(type), _intervals(orderIntervals)
{
	_variableSectionOffset = config._blockSize;
	_isDone = false;
}

AbstractNode::~AbstractNode()
{
}

AbstractNode::AbstractNode(HistoryTreeConfig config)
: _config(config), _intervals(orderIntervals) {
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
void AbstractNode::writeInfoFromNode(vector<AbstractInterval::SharedPtr>& intervals, timestamp_t timestamp) const
{
	int startIndex;

	if ( _intervals.size() == 0 ) { return; }

	for (IntervalContainer::const_iterator it = getStartIndexFor(timestamp); it != _intervals.end(); it++ ) {
		/* Now we only have to compare the Start times, since we know
		 * the End times necessarily fit */
		if ( (*it)->getStart() <= timestamp ) {
			if ((int) intervals.size() < (*it)->getAttribute()+1) 
				intervals.resize((*it)->getAttribute()+1);
			intervals[(*it)->getAttribute()] = (*it);
		}
	}
	return;
}

/**
 * Add an interval to this node
 * @param newInterval
 */
void AbstractNode::addInterval(AbstractInterval::SharedPtr newInterval)
{
	// Just in case, but should be checked before even calling this function
	assert(newInterval->getTotalSize() <= getFreeSpace());
	
	// FIXME : We need to clone the interval, to guarantee ownership
	_intervals.insert(newInterval);
	
	// Update the in-node offset "pointer"
	_variableSectionOffset -= (newInterval->getVariableValueSize());
}

/**
 * We've received word from the containerTree that newest nodes now exist to
 * our right. (Puts isDone = true and sets the endtime)
 * 
 * @param endtime The nodeEnd time that the node will have
 * @throw TimeRangeException 
 */
void AbstractNode::close(timestamp_t endtime)
{
	assert ( endtime >= _nodeStart );
	
	_isDone = true;
	_nodeEnd = endtime;
	return;	
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
AbstractInterval::SharedPtr AbstractNode::getRelevantInterval(timestamp_t timestamp, attribute_t key) const
{
	assert ( _isDone );
		
	if ( _intervals.size() == 0 ) { return AbstractInterval::SharedPtr(); }
	
	for ( IntervalContainer::const_iterator it = getStartIndexFor(timestamp); it != _intervals.end(); it++ ) {
		if ( (*it)->getAttribute() == key ) {
			if ( (*it)->getStart() <= timestamp ) {
				return (*it);
			}
		}
	}
	/* We didn't find the relevant information in this node */
	return AbstractInterval::SharedPtr();
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
IntervalContainer::const_iterator AbstractNode::getStartIndexFor(timestamp_t timestamp) const
{
	static AbstractInterval::SharedPtr dummyInterval(new NullInterval(0, 0, 0));
	dummyInterval->setEnd(timestamp);
	IntervalContainer::const_iterator it;	
	
	it = _intervals.lower_bound(dummyInterval);
	
	return it;
}

/**
 * Returns the free space in the node, which is simply put,
 * the stringSectionOffset - dataSectionOffset
 */
unsigned int AbstractNode::getFreeSpace() const
{
	return _variableSectionOffset - getDataSectionEndOffset();
}

unsigned int AbstractNode::getTotalHeaderSize() const
{
	return AbstractNode::COMMON_HEADER_SIZE + this->getSpecificHeaderSize();
}

/**
 * @return The offset, within the node, where the Data (intervals) section ends
 */
int AbstractNode::getDataSectionEndOffset() const
{
	return getTotalHeaderSize() + AbstractInterval::getHeaderSize() * _intervals.size();
}

void AbstractNode::serialize(ostream& os)
{
	// allocate some byte buffer
	// TODO: private buffer to avoid new/delete for each block write?
	const unsigned int bufsz = this->_config._blockSize;
	uint8_t* buf = new uint8_t [bufsz];
	
	// serialize to buffer
	this->serialize(buf);
	
	// write to output
	os.write((char*) buf, bufsz);
	
	// free buffer
	delete [] buf;
}

void AbstractNode::serialize(uint8_t* buf)
{
	// pointer backup
	uint8_t* bkbuf = buf;
	
	// prepare common header fields
	int32_t interval_count = (int32_t) this->_intervals.size();
	int32_t var_sect_offset = (int32_t) _variableSectionOffset;
	uint8_t isDone = this->_isDone ? 1 : 0;
	
	// write block common header
	memcpy(buf, &this->_type, sizeof(node_type_t));
	buf += sizeof(node_type_t);
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
	this->serializeSpecificHeader(buf);
	buf += this->getSpecificHeaderSize();

	// write intervals (OO fashion)
	IntervalContainer::iterator it;
	uint8_t* var_addr = bkbuf + this->_config._blockSize;
	for (it = this->_intervals.begin(); it != this->_intervals.end(); ++it) {
		AbstractInterval::SharedPtr interval = *it;
		// get interval variable value size
		unsigned int var_size = interval->getVariableValueSize();
		var_addr -= var_size;
		unsigned int offset_ptr = (unsigned int) (var_addr - bkbuf);
		
		// serialize interval
		interval->serialize(var_addr, buf);
		
		// overwrite header's value with variable value "pointer" if variable size isn't 0
		if (var_size != 0) {
			memcpy(buf + interval->getHeaderSize() - sizeof(uint32_t), &offset_ptr, sizeof(uint32_t));
		}
		buf += interval->getHeaderSize();
	}
}

void AbstractNode::unserialize(std::istream& is, const IntervalCreator& ic) {
	// remember initial position within stream
	unsigned int init_pos = is.tellg();
	
	// type
	is.read((char*) &this->_type, sizeof(node_type_t));
	
	// time stamps
	is.read((char*) &this->_nodeStart, sizeof(timestamp_t));
	is.read((char*) &this->_nodeEnd, sizeof(timestamp_t));
	
	// sequence numbers
	is.read((char*) &this->_sequenceNumber, sizeof(seq_number_t));
	is.read((char*) &this->_parentSequenceNumber, sizeof(seq_number_t));
	
	// interval count
	int32_t interval_count;
	is.read((char*) &interval_count, sizeof(int32_t));
	
	// skip var. data offset and done (will be deduced when parsing intervals)
	is.seekg(sizeof(int32_t) + sizeof(uint8_t), ios::cur);
	this->_isDone = true;
	
	// at specific header now
	this->unserializeSpecificHeader(is);
	
	// unserialize intervals
	unsigned int len = this->_config._blockSize - this->getTotalHeaderSize();
	uint8_t* const buf = new uint8_t [len];
	is.seekg(init_pos + this->getTotalHeaderSize(), ios::beg);
	is.read((char*) buf, len);
	this->_variableSectionOffset = this->_config._blockSize;
	uint8_t* datPtr = buf;
	uint8_t* varPtr = buf + len;
	for (int i = 0; i < interval_count; ++i) {
		// get the appropriate interval from the creator
		interval_type_t type = datPtr[20]; // TODO: make this prettier
		AbstractInterval::SharedPtr interval = ic.createIntervalFromType(type);
		
		// unserialize it
		unsigned int var_len = interval->unserialize(varPtr, datPtr);
		assert(var_len == interval->getVariableValueSize());
		
		// keep it...
		this->_intervals.insert(interval);
		
		// new buffer offsets
		datPtr += AbstractInterval::getHeaderSize();
		varPtr -= var_len;
		this->_variableSectionOffset -= var_len;
	}
	
	// clean
	delete [] buf;
}

std::string AbstractNode::toString(void) const {
	ostringstream oss;
	stringstream endTime;
	if(_isDone){
		endTime << _nodeEnd;
	}else{
		endTime << "...";
	}
	oss << "# {" << this->_sequenceNumber << "} from {" << this->_parentSequenceNumber << "} " <<
		"[" << this->_nodeStart << ", " << endTime.str() << "], " << this->_intervals.size() <<
		" intervals" << this->getInfos();
	
	// intervals
	oss << endl;
	for (IntervalContainer::const_iterator it = this->_intervals.begin(); it != this->_intervals.end(); ++it) {
		oss << "  " << *(*it) << endl;
	}

	return oss.str();
}

std::ostream& operator<<(std::ostream& out, const AbstractNode& node) {
	out << node.toString();
	
	return out;
}
