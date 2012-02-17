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
#ifndef _HISTORYTREENODE_HPP
#define _HISTORYTREENODE_HPP

#include "Interval.hpp"
#include "basic_types.h"

#include <stdint.h>
#include <ostream>
#include <vector>

class HistoryTree;

class HistoryTreeNode
{
public:
	HistoryTreeNode();
	HistoryTreeNode(HistoryTree& tree, int seqNumber, int parentSeqNumber, timestamp_t start);
	virtual ~HistoryTreeNode();
	static HistoryTreeNode readNode(const HistoryTree& tree);
	
	void writeInfoFromNode(std::vector<std::tr1::shared_ptr<Interval> >& intervals, timestamp_t timestamp) const;
	
	void serialize(uint8_t* buf);
	
	void serialize(std::ostream& os);
	
	virtual unsigned int writeHeader(uint8_t* buf) { return 0; }
	
	void addInterval(const Interval& newInterval);
	
	void closeThisNode(timestamp_t endtime);
	
	void linkNewChild(const HistoryTreeNode& childNode);
	
	std::tr1::shared_ptr<Interval> getRelevantInterval(timestamp_t timestamp, attribute_t key) const;
	
	int getStartIndexFor(timestamp_t timestamp) const;
	
	unsigned int getFreeSpace();
	
	int getTotalHeaderSize() const;
	
	timestamp_t getNodeStart() const {return _nodeStart;}
	
	timestamp_t getNodeEnd() const 
	{		
		if ( _isDone ) {
			return _nodeEnd;
		} else {
			return 0;
		}
	}
	
	int getNbChildren() const
	{
		return _nbChildren;
	}
	
	int getSequenceNumber() const
	{
		return _sequenceNumber;
	}
	
	int getParentSequenceNumber() {
		return _parentSequenceNumber;
	}
	
	void setParentSequenceNumber(int newParent) {
		_parentSequenceNumber = newParent;
	}	
	
	int getChild(int index) const
	{
		return _children[index];
	}
	
	timestamp_t getChildStart(int index) const
	{
		return _childStart[index];
	}
	
	bool isDone() const
	{
		return _isDone;
	}

private:
	/* Pointer to the History Tree to whom this node belongs */
	HistoryTree* _ownerTree;
	
	/* Time range of this node */
	timestamp_t _nodeStart;
	timestamp_t _nodeEnd;
	
	/* Sequence number = position in the node section of the file */
	seq_number_t _sequenceNumber;
	seq_number_t _parentSequenceNumber; /* = -1 if this node is the root node */
	
	/* Where the Strings section begins (from the start of the node */
	unsigned int _variableSectionOffset;
	
	/* True if this node is closed (and to be committed to disk) */
	bool _isDone;
	
	/* Vector containing all the intervals contained in this node */
	std::vector<std::tr1::shared_ptr<Interval> > _intervals;

	int _nbChildren;	/* Nb. of children this node has */
	int* _children;		/* Seq. numbers of the children nodes (size = MAX_NB_CHILDREN) */
	timestamp_t* _childStart;	/* Start times of each of the children (size = MAX_NB_CHILDREN) */
	uint8_t _nodeType;
	
	int getDataSectionEndOffset() const;
};

#endif // _HISTORYTREENODE_HPP
