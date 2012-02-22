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

#include "HistoryTreeIO.hpp"

#if 1

/**
 * Empty constructor
 * 
 * @param tree
 */
HistoryTreeIO::HistoryTreeIO()
:_tree(0)
{
}

/**
 * "New history file" constructor
 * 
 * @param tree
 */
HistoryTreeIO::HistoryTreeIO(HistoryTree* ownerTree)
:_tree(ownerTree)
{
	//FIXME magic IO init goes here
}

/**
 * "Existing history file" constructor
 * 
 * @param ownerTree
 * @param curNodeCount used to distinguish from new history file constructor (ugly)
 */
HistoryTreeIO::HistoryTreeIO(HistoryTree* ownerTree, int curNodeCount)
:_tree(ownerTree)
{
	//FIXME magic IO init goes here
}

/**
 * Generic "read node" method, which checks if the node is in memory first,
 * and if it's not it goes to disk to retrieve it.
 * 
 * @param seqNumber
 *            Sequence number of the node we want
 * @return The wanted node in object form
 */
HistoryTreeNodeSharedPtr HistoryTreeIO::readNode(int seqNumber) const
{
	HistoryTreeNodeSharedPtr node;
	try{
		node = readNodeFromMemory(seqNumber);
	}catch(...){
		return readNodeFromDisk(seqNumber);
	}
	return node;
}

HistoryTreeNodeSharedPtr HistoryTreeIO::readNodeFromMemory(int seqNumber) const
{
	//FIXME implement this
	
	//for (HTNode node : tree.latestBranch) {
		//if (node.getSequenceNumber() == seqNumber) {
			//return node;
		//}
	//}
	//return null;
	return HistoryTreeNodeSharedPtr();
}

HistoryTreeNodeSharedPtr HistoryTreeIO::readNodeFromDisk(int seqNumber) const
{
	//FIXME implement this
	
	//HTNode readNode;
	//try {
		//seekFCToNodePos(fcIn, seqNumber);
		//readNode = HTNode.readNode(tree, fcIn);
		//return readNode;
	//} catch (IOException e) {
		//e.printStackTrace();
		//return null;
	//}
	return HistoryTreeNodeSharedPtr();
}

void HistoryTreeIO::writeNode(HistoryTreeNodeSharedPtr node)
{
	//FIXME implement this
	
//	try {
//		/* Position ourselves at the start of the node and write it */
//		seekFCToNodePos(fcOut, node.getSequenceNumber());
//		node.writeSelf(fcOut);
//	} catch (IOException e) {
//		/* If we were able to open the file, we should be fine now... */
//		e.printStackTrace();
//	}	
}

#endif
