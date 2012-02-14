#include "HistoryTreeNode.hpp"
#include "HistoryTree.hpp"

#include <algorithm>
#include <assert.h>

using namespace std;
using namespace std::tr1;

HistoryTreeNode::HistoryTreeNode()
:_nbChildren(0)
{
}

HistoryTreeNode::HistoryTreeNode(HistoryTree& tree, int seqNumber, int parentSeqNumber, uint64_t start)
:_ownerTree(&tree), _nodeStart(start), _sequenceNumber(seqNumber), _parentSequenceNumber(parentSeqNumber), _nbChildren(0)
{
	_variableSectionOffset = _ownerTree->getConfig()._blockSize;
	_isDone = false;
	_intervals = std::vector<shared_ptr<Interval> >();
}

HistoryTreeNode::~HistoryTreeNode()
{
}

HistoryTreeNode HistoryTreeNode::readNode(const HistoryTree& tree)
{
	return HistoryTreeNode();
}

void HistoryTreeNode::writeInfoFromNode(vector<shared_ptr<Interval> >& intervals, uint64_t timestamp) const
{
	int startIndex;

	if ( _intervals.size() == 0 ) { return; }
	startIndex = getStartIndexFor(timestamp);

	for ( unsigned int i = startIndex; i < _intervals.size(); i++ ) {
		/* Now we only have to compare the Start times, since we now
		 * the End times necessarily fit */
		if ( _intervals[i]->getStart() <= timestamp ) {
			intervals[_intervals[i]->getAttribute()] = _intervals[i];
		}
	}
	return;
}

void HistoryTreeNode::addInterval(const Interval& newInterval)
{
	/* Just in case, but should be checked before even calling this function */
	assert( newInterval.getIntervalSize() <= getNodeFreeSpace() );
	
	// FIXME we need a way to duplicate the abstract interval
	//_intervals.push_back( newInterval );
	
	/* Update the in-node offset "pointer" */
	_variableSectionOffset -= ( newInterval.getVariableEntrySize() );	
}

void HistoryTreeNode::closeThisNode(uint64_t endtime)
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
		 //FIXME uncomment this and make it work!
		//std::sort(_intervals.begin(), _intervals.end(), std::less<Interval>());
		
		/* Make sure there are no intervals in this node with their
		 * EndTime > the one requested. Only need to check the last one
		 * since they are now sorted */
		assert ( endtime >= _intervals[_intervals.size()-1]->getEnd() );
	}
	
	_isDone = true;
	_nodeEnd = endtime;
	return;	
}

void HistoryTreeNode::linkNewChild(const HistoryTreeNode& childNode)
{
	assert( _nbChildren < _ownerTree->getConfig()._maxChildren );
	
	_children[_nbChildren] = childNode.getSequenceNumber();
	_childStart[_nbChildren] = childNode.getNodeStart();
	_nbChildren++;	
}


int HistoryTreeNode::getStartIndexFor(uint64_t timestamp) const
{
	//FIXME Use a binary search algorithm to find the correct index
	return 0;
}

int HistoryTreeNode::getNodeFreeSpace()
{
	return _variableSectionOffset - getDataSectionEndOffset();
}

int HistoryTreeNode::getTotalHeaderSize() const
{
	int headerSize = 0;
	
	headerSize +=	  4	/* 1x int (extension node) */
			+ 4	/* 1x int (nbChildren) */
			+ 4 * _ownerTree->getConfig()._maxChildren	/* MAX_NB * int ('children' table) */
			+ 8 * _ownerTree->getConfig()._maxChildren	/* MAX_NB * Timevalue ('childStart' table) */
			+ 1 	/* byte (type) */
			+ 16	/* 2x long (start time, end time) */
			+ 16	/* 4x int (seq number, parent seq number, intervalcount, strings section pos.) */
			+ 1;	/* byte (done or not) */
	return headerSize;
}

int HistoryTreeNode::getDataSectionEndOffset() const
{
	return getTotalHeaderSize() + Interval::getStaticEntrySize() * _intervals.size();
}
