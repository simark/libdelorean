#include "HistoryTree.hpp"

#include <assert.h>

HistoryTree::HistoryTree()
{
}

HistoryTree::~HistoryTree()
{
}

void HistoryTree::insertInterval(const Interval& interval)
{	/* FIXME do this throw
	if ( interval.getStartTime() < config.treeStart ) {
		throw new TimeRangeException();
	}
	*/
	
	tryInsertAtNode(interval, _latestBranch.size()-1);
}

void HistoryTree::tryInsertAtNode(const Interval& interval, int indexOfNode)
{
	HistoryTreeNode& targetNode(_latestBranch[indexOfNode]);
	
	/* Verify if there is enough room in this node to store this interval */
	if ( interval.getIntervalSize() > targetNode.getNodeFreeSpace() ) {
		/* Nope, not enough room. Insert in a new sibling instead. */
		addSiblingNode(indexOfNode);
		tryInsertAtNode(interval, _latestBranch.size()-1);
		return;
	}
	
	/* Make sure the interval time range fits this node */
	if ( interval.getStart() < targetNode.getNodeStart() ) {
		/* No, this interval starts before the startTime of this node.
		 * We need to check recursively in parents if it can fit. */
		assert ( indexOfNode >= 1 );
		tryInsertAtNode( interval, indexOfNode-1 );
		return;
	}
	
	/* Ok, there is room, and the interval fits in this time slot. Let's add it. */
	targetNode.addInterval(interval);
	
	/* Update treeEnd if needed */
	if ( interval.getEnd() > _treeEnd ) {
		_treeEnd = interval.getEnd();
	}
	return;
}


HistoryTreeNode HistoryTree::selectNextChild(const HistoryTreeNode& currentNode, uint64_t timestamp) const
{
	assert ( currentNode.getNbChildren() > 0 );
	int potentialNextSeqNb = currentNode.getSequenceNumber();
	
	for ( int i = 0; i < currentNode.getNbChildren(); i++ ) {
		if ( timestamp >= currentNode.getChildStart(i) ) {
			potentialNextSeqNb = currentNode.getChild(i);
		} else {
			break;
		}
	}
	/* Once we exit this loop, we should have found a children to follow.
	 * If we didn't, there's a problem. */
	assert ( potentialNextSeqNb != currentNode.getSequenceNumber() );
	
	/* Since this code path is quite performance-critical, avoid iterating
	 * through the whole latestBranch array if we know for sure the next
	 * node has to be on disk */
	if ( currentNode.isDone() ) {
		return _treeIO.readNodeFromDisk(potentialNextSeqNb);
	} else {
		return _treeIO.readNode(potentialNextSeqNb);
	}
}
