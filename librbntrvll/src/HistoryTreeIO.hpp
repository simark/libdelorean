#ifndef _HISTORYTREEIO_HPP
#define _HISTORYTREEIO_HPP

#include "HistoryTreeNode.hpp"

class HistoryTreeIO
{
public:
	HistoryTreeNode readNode(int seqNumber) const;
	HistoryTreeNode readNodeFromMemory(int seqNumber) const;
	HistoryTreeNode readNodeFromDisk(int seqNumber) const;
	
	void writeNode(HistoryTreeNode& node);

private:

};

#endif // _HISTORYTREEIO_HPP
