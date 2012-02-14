#ifndef _HISTORYTREENODE_HPP
#define _HISTORYTREENODE_HPP

#include "Interval.hpp"

#include <stdint.h>
#include <vector>

class HistoryTree;

class HistoryTreeNode
{
public:
	HistoryTreeNode();
	HistoryTreeNode(HistoryTree& tree, int seqNumber, int parentSeqNumber, uint64_t start);
	virtual ~HistoryTreeNode();
	static HistoryTreeNode readNode(const HistoryTree& tree);
	
	void writeInfoFromNode(std::vector<std::tr1::shared_ptr<Interval> >& intervals, uint64_t timestamp) const;
	
	void addInterval(const Interval& newInterval);
	
	void closeThisNode(uint64_t endtime);
	
	void linkNewChild(const HistoryTreeNode& childNode);
	
	int getStartIndexFor(uint64_t timestamp) const;
	
	int getNodeFreeSpace();
	
	int getTotalHeaderSize() const;
	
	uint64_t getNodeStart() const {return _nodeStart;}
	
	uint64_t getNodeEnd() const 
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
	
	uint64_t getChildStart(int index) const
	{
		return _childStart[index];
	}
	
	bool isDone() const
	{
		return _isDone;
	}
	
private:
	/* Reference to the History Tree to whom this node belongs */
	HistoryTree* _ownerTree;
	
	/* Time range of this node */
	uint64_t _nodeStart;
	uint64_t _nodeEnd;
	
	/* Sequence number = position in the node section of the file */
	int _sequenceNumber;
	int _parentSequenceNumber; /* = -1 if this node is the root node */
	
	/* Where the Strings section begins (from the start of the node */
	int _variableSectionOffset;
	
	/* True if this node is closed (and to be committed to disk) */
	bool _isDone;
	
	/* Vector containing all the intervals contained in this node */
	std::vector<std::tr1::shared_ptr<Interval> > _intervals;

	int _nbChildren;	/* Nb. of children this node has */
	int* _children;		/* Seq. numbers of the children nodes (size = MAX_NB_CHILDREN) */
	uint64_t* _childStart;	/* Start times of each of the children (size = MAX_NB_CHILDREN) */
	
	
	int getDataSectionEndOffset() const;
};

#endif // _HISTORYTREENODE_HPP
