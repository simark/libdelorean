#ifndef _HISTORYTREENODE_HPP
#define _HISTORYTREENODE_HPP

class HistoryTree;

class HistoryTreeNode
{
public:
	HistoryTreeNode();
	virtual ~HistoryTreeNode();
	static HistoryTreeNode readNode(const HistoryTree& tree);
};

#endif // _HISTORYTREENODE_HPP
