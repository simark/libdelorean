#include "HistoryTree.hpp"

#include <assert.h>

HistoryTree::HistoryTree()
{
}

HistoryTree::~HistoryTree()
{
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
