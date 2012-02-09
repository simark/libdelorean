#include "HistoryTreeNode.hpp"
#include "HistoryTree.hpp"

HistoryTreeNode::HistoryTreeNode()
:nbChildren(0)
{
}

HistoryTreeNode::~HistoryTreeNode()
{
}

HistoryTreeNode HistoryTreeNode::readNode(const HistoryTree& tree)
{
	return HistoryTreeNode();
}
