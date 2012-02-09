#ifndef _HISTORYTREE_HPP
#define _HISTORYTREE_HPP

#include "HistoryTreeNode.hpp"
#include "HistoryTreeConfig.hpp"
#include "HistoryTreeIO.hpp"

class HistoryTree
{
public:
	HistoryTree();
	virtual ~HistoryTree();
	
	HistoryTreeNode selectNextChild(const HistoryTreeNode& currentNode, uint64_t timestamp) const;
		
	//FIXME const?
	const std::vector<HistoryTreeNode>& getLatestBranch() const
	{
		return _latestBranch;
	}
	
	uint64_t getTreeStart() const
	{
		return _config._treeStart;
	}
	
	uint64_t getTreeEnd() const
	{
		return treeEnd;
	}
	
	uint64_t getNodeCount() const
	{
		return nodeCount;
	}
private:
	HistoryTreeConfig _config;
	HistoryTreeIO _treeIO;
	
	uint64_t treeEnd;	/* Latest timestamp found in the tree (at any given moment) */
	int nodeCount;		/* How many nodes exist in this tree, total */
	
	std::vector<HistoryTreeNode> _latestBranch;
};

#endif // _HISTORYTREE_HPP
