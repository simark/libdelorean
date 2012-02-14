#ifndef _HISTORYTREE_HPP
#define _HISTORYTREE_HPP

#include "HistoryTreeNode.hpp"
#include "HistoryTreeConfig.hpp"
#include "HistoryTreeIO.hpp"
#include "Interval.hpp"

class HistoryTree
{
public:
	HistoryTree();
	virtual ~HistoryTree();
	
	void insertInterval(const Interval& interval);
	
	HistoryTreeNode selectNextChild(const HistoryTreeNode& currentNode, uint64_t timestamp) const;
		
	//FIXME const?
	const std::vector<HistoryTreeNode>& getLatestBranch() const
	{
		return _latestBranch;
	}
	
	const HistoryTreeConfig& getConfig() const
	{
		return _config;
	}
	
	uint64_t getTreeStart() const
	{
		return _config._treeStart;
	}
	
	uint64_t getTreeEnd() const
	{
		return _treeEnd;
	}
	
	uint64_t getNodeCount() const
	{
		return _nodeCount;
	}
private:
	HistoryTreeConfig _config;
	HistoryTreeIO _treeIO;
	
	uint64_t _treeEnd;	/* Latest timestamp found in the tree (at any given moment) */
	int _nodeCount;		/* How many nodes exist in this tree, total */
	
	std::vector<HistoryTreeNode> _latestBranch;
	
	void tryInsertAtNode(const Interval& interval, int indexOfNode);
	
	void addSiblingNode(unsigned int indexOfNode);
	
	void addNewRootNode();
	
	HistoryTreeNode initNewCoreNode(int parentSeqNumber, uint64_t startTime);
};

#endif // _HISTORYTREE_HPP
