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
#ifndef _ABSTRACTNODE_HPP
#define _ABSTRACTNODE_HPP

#include <stdint.h>
#include <ostream>
#include <vector>
#include <set>
#include <string>
#include <map>

#include "IPrintable.hpp"
#include "intervals/AbstractInterval.hpp"
#include "IntervalCreator.hpp"
#include "HistoryTreeConfig.hpp"
#include "basic_types.h"

#include "ex/TimeRangeEx.hpp"

#include <boost/thread/shared_mutex.hpp>

typedef std::multiset<AbstractInterval::SharedPtr, bool (*)(AbstractInterval::SharedPtr, AbstractInterval::SharedPtr)> IntervalContainer;

class AbstractNode : public IPrintable
{
	friend std::ostream& operator<<(std::ostream& out, const AbstractNode& node);
	
public:
	typedef std::tr1::shared_ptr<AbstractNode> SharedPtr;
	typedef std::tr1::shared_ptr<const AbstractNode> ConstSharedPtr;

	AbstractNode(HistoryTreeConfig config);
	AbstractNode(HistoryTreeConfig config, seq_number_t seqNumber, seq_number_t parentSeqNumber, timestamp_t start, node_type_t type);
	virtual ~AbstractNode();
	std::string toString(void) const;
	virtual std::string getInfos(void) const = 0;
	void writeInfoFromNode(std::vector<AbstractInterval::SharedPtr>& intervals, timestamp_t timestamp) const;
	void writeInfoFromNode(std::multimap<attribute_t, AbstractInterval::SharedPtr>& intervals, timestamp_t timestamp) const;
	void serialize(uint8_t* buf);
	void serialize(std::ostream& os);
	void unserialize(std::istream& is, const IntervalCreator& ic);
	virtual void unserializeSpecificHeader(std::istream& is) = 0;
	virtual void serializeSpecificHeader(uint8_t* buf) const = 0;
	virtual unsigned int getSpecificHeaderSize(void) const = 0;
	void addInterval(AbstractInterval::SharedPtr) throw(TimeRangeEx);
	void close(timestamp_t endtime);
	AbstractInterval::SharedPtr getRelevantInterval(timestamp_t timestamp, attribute_t key) const;
	IntervalContainer::const_iterator getStartIndexFor(timestamp_t timestamp) const;
	unsigned int getFreeSpace() const;
	unsigned int getTotalHeaderSize() const;
	timestamp_t getStart() const {
		return _nodeStart;
	}
	timestamp_t getEnd() const {
		return _nodeEnd;
	}
	seq_number_t getSequenceNumber() const {
		return _sequenceNumber;
	}
	seq_number_t getParentSequenceNumber() {
		return _parentSequenceNumber;
	}
	void setParentSequenceNumber(seq_number_t newParent) {
		_parentSequenceNumber = newParent;
	}
	bool isDone() const {
		return _isDone;
	}
	void reopen(){
		_isDone = false;
	}
	
	static const unsigned int COMMON_HEADER_SIZE;

protected:
	// owner tree's configuration
	HistoryTreeConfig _config;
	
	// time range of this node
	timestamp_t _nodeStart;
	timestamp_t _nodeEnd;
	
	// sequence number = position in the node section of the file
	seq_number_t _sequenceNumber;
	seq_number_t _parentSequenceNumber; // -1 if this node is the root node
	
	// variable length data section's offset (0 being the beggining of the node block)
	unsigned int _variableSectionOffset;
	
	// true if this node is closed (ready to be committed to disk)
	bool _isDone;
	
	// node type
	node_type_t _type;
	
	// vector containing all the node intervals
	IntervalContainer _intervals;
	
	// mutex to prevent changes to a node while it is being accessed
	mutable boost::shared_mutex _mutex;
	
	int getDataSectionEndOffset() const;
};

#endif // _ABSTRACTNODE_HPP
