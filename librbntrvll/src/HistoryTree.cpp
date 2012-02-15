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
#include "ex/TimeRangeEx.hpp"

#include <assert.h>

/**
 * Create a new HistoryTree from scratch, using a HistoryTreeConfig object
 * for configuration
 * 
 * @param config
 */
HistoryTree::HistoryTree(HistoryTreeConfig config)
:_config(config)
{
	/* Simple assertion to make sure we have enough place
	 * in the 0th block for the tree configuration */
	assert( _config._blockSize >= getTreeHeaderSize() );
	
	_treeEnd = _config._treeStart;
	_nodeCount = 0;
	_latestBranch = std::vector<HistoryTreeNode>();
	
	/* Prepare the IO object */
	_treeIO = HistoryTreeIO(this);
	
	/* Add the first node to the tree */
	HistoryTreeNode firstNode = initNewCoreNode(-1, _config._treeStart);
	_latestBranch.push_back(firstNode);	
}

/**
 * Create a new HistoryTree from scratch, using parameters needed to
 * construct a HistoryTreeConfig object
 * 
 * @param newFile
 * @param blockSize
 * @param maxChildren
 * @param startTime
 */
HistoryTree::HistoryTree(std::string newFile, int blockSize, int maxChildren, timestamp_t startTime)
:_config(newFile, blockSize, maxChildren, startTime)
{
	/* Simple assertion to make sure we have enough place
	 * in the 0th block for the tree configuration */
	assert( _config._blockSize >= getTreeHeaderSize() );
	
	_treeEnd = _config._treeStart;
	_nodeCount = 0;
	_latestBranch = std::vector<HistoryTreeNode>();
	
	/* Prepare the IO object */
	_treeIO = HistoryTreeIO(this);
	
	/* Add the first node to the tree */
	HistoryTreeNode firstNode = initNewCoreNode(-1, _config._treeStart);
	_latestBranch.push_back(firstNode);	
}

/**
 * "Reader" constructor : instantiate a HistoryTree from an existing tree file on
 * disk
 * 
 * @param existingFile
 *            Path/filename of the history-file we are to open
 */
HistoryTree::HistoryTree(std::string existingFile)
{
	//FIXME put magic I/O sequence here
	
	_treeIO = HistoryTreeIO(this);
}

HistoryTree::~HistoryTree()
{
}

/**
 * "Save" the tree to disk.
 * This method will cause the treeIO object to commit all nodes to disk
 * and then return the RandomAccessFile descriptor so the Tree object can
 * save its configuration into the header of the file.
 * @throws TimeRangeException 
 * 
 */
void HistoryTree::closeTree(timestamp_t timestamp)
{
	//FIXME do the complex work here
}

/**
 * Insert an interval in the tree
 * 
 * @param interval
 */
void HistoryTree::insertInterval(const Interval& interval)
{
	if ( interval.getStart() < _config._treeStart ) {
		throw TimeRangeEx("Interval start time below IntervalTree start time");
	}
	
	tryInsertAtNode(interval, _latestBranch.size()-1);
}

/**
 * Inner method to select the next child of the current node intersecting
 * the given timestamp. Useful for moving down the tree following one
 * branch.
 * 
 * @param currentNode
 * @param t
 * @return The child node intersecting t
 */
HistoryTreeNode HistoryTree::selectNextChild(const HistoryTreeNode& currentNode, timestamp_t timestamp) const
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

/**
 * Helper function to get the size of the "tree header" in the tree-file
 * The nodes will use this offset to know where they should be in the file.
 * This should always be a multiple of 4K.
 */
static int getTreeHeaderSize() {
	return 4096;
}

/**
 * Inner method to find in which node we should add the interval.
 * 
 * @param interval
 *            The interval to add to the tree
 * @param indexOfNode
 *            The index *in the latestBranch* where we are trying the
 *            insertion
 */
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

/**
 * Method to add a sibling to any node in the latest branch. This will add
 * children back down to the leaf level, if needed.
 * 
 * @param indexOfNode
 *            The index in latestBranch where we start adding
 */
void HistoryTree::addSiblingNode(unsigned int indexOfNode)
{
	unsigned int i;
	HistoryTreeNode newNode, prevNode;
	timestamp_t splitTime = _treeEnd;
	
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

/**
 * Similar to the previous method, except here we rebuild a completely new
 * latestBranch
 */
void HistoryTree::addNewRootNode()
{
	unsigned int i, depth;
	timestamp_t splitTime = _treeEnd;
	
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

/**
 * Add a new empty node to the tree.
 * 
 * @param parentSeqNumber
 *            Sequence number of this node's parent
 * @param startTime
 *            Start time of the new node
 * @return The newly created node
 */
HistoryTreeNode HistoryTree::initNewCoreNode(int parentSeqNumber, timestamp_t startTime)
{
	HistoryTreeNode newNode(*this, _nodeCount, parentSeqNumber, startTime);
	_nodeCount++;
	
	/* Update the treeEnd if needed */
	if ( startTime >= _treeEnd ) {
		_treeEnd = startTime + 1;
	}
	return newNode;	
}
