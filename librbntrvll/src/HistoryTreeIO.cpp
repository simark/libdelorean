#include "HistoryTreeIO.hpp"

/**
 * Generic "read node" method, which checks if the node is in memory first,
 * and if it's not it goes to disk to retrieve it.
 * 
 * @param seqNumber
 *            Sequence number of the node we want
 * @return The wanted node in object form
 */
HistoryTreeNode HistoryTreeIO::readNode(int seqNumber) const
{
	HistoryTreeNode node;
	try{
		node = readNodeFromMemory(seqNumber);
	}catch(...){
		return readNodeFromDisk(seqNumber);
	}
	return node;
}

HistoryTreeNode HistoryTreeIO::readNodeFromMemory(int seqNumber) const
{
	//FIXME implement this
	
	//for (HTNode node : tree.latestBranch) {
		//if (node.getSequenceNumber() == seqNumber) {
			//return node;
		//}
	//}
	//return null;
	return HistoryTreeNode();
}

HistoryTreeNode HistoryTreeIO::readNodeFromDisk(int seqNumber) const
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
	return HistoryTreeNode();
}

void HistoryTreeIO::writeNode(HistoryTreeNode& node)
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
