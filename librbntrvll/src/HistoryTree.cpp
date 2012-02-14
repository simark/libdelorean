/**
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

void HistoryTree::addSiblingNode(unsigned int indexOfNode)
{
	unsigned int i;
	HistoryTreeNode newNode, prevNode;
	uint64_t splitTime = _treeEnd;
	
	assert ( indexOfNode < _latestBranch.size() );
	
	/* Check if we need to add a new root node */
	if ( indexOfNode == 0 ) {
		addNewRootNode();
		return;
	}
	
	/* Check if we can indeed add a child to the target parent */
	if ( _latestBranch[indexOfNode-1].getNbChildren() == _config._maxChildren ) {
		/* If not, add a branch starting one level higher instead */
		addSiblingNode(indexOfNode-1);
		return;
	}
	
	/* Split off the new branch from the old one */
	for ( i=indexOfNode ; i < _latestBranch.size(); i++ ) {
		_latestBranch[i].closeThisNode(splitTime);
		_treeIO.writeNode(_latestBranch[i]);
		
		prevNode = _latestBranch[(i-1)];
		newNode = initNewCoreNode(prevNode.getSequenceNumber(), splitTime + 1);
		prevNode.linkNewChild(newNode);
		
		_latestBranch[i] = newNode;
	}
	return;
}

void HistoryTree::addNewRootNode()
{
	unsigned int i, depth;
	uint64_t splitTime = _treeEnd;
	
	HistoryTreeNode& oldRootNode(_latestBranch[0]);
	HistoryTreeNode newRootNode = initNewCoreNode(-1, _config._treeStart);
	
	/* Tell the old root node that it isn't root anymore */
	oldRootNode.setParentSequenceNumber(newRootNode.getSequenceNumber());
	
	/* Close off the whole current latestBranch */
	for ( i=0; i < _latestBranch.size(); i++ ) {
		_latestBranch[i].closeThisNode(splitTime);
		_treeIO.writeNode(_latestBranch[i]);
	}
	
	/* Link the new root to its first child (the previous root node) */
	newRootNode.linkNewChild(oldRootNode);
	
	/* Rebuild a new latestBranch */
	depth = _latestBranch.size();
	_latestBranch = std::vector<HistoryTreeNode>();
	_latestBranch.push_back(newRootNode);
	for ( i=1; i < depth+1; i++ ) {
		HistoryTreeNode& prevNode(_latestBranch[i-1]);
		HistoryTreeNode newNode = initNewCoreNode(prevNode.getParentSequenceNumber(), splitTime + 1);
		prevNode.linkNewChild(newNode);
		_latestBranch.push_back(newNode);
	}
}

HistoryTreeNode HistoryTree::initNewCoreNode(int parentSeqNumber, uint64_t startTime)
{
	HistoryTreeNode newNode(*this, _nodeCount, parentSeqNumber, startTime);
	_nodeCount++;
	
	/* Update the treeEnd if needed */
	if ( startTime >= _treeEnd ) {
		_treeEnd = startTime + 1;
	}
	return newNode;	
}
