#ifndef _HISTORYTREENODE_HPP
#define _HISTORYTREENODE_HPP

#include <stdint.h>

class HistoryTree;

class HistoryTreeNode
{
public:
	HistoryTreeNode();
	virtual ~HistoryTreeNode();
	static HistoryTreeNode readNode(const HistoryTree& tree);
	
	int getNbChildren() const
	{
		return _nbChildren;
	}
	
private:
	/* Reference to the History Tree to whom this node belongs */
	HistoryTree* ownerTree;
	
	/* Time range of this node */
	uint64_T _nodeStart;
	uint64_t _nodeEnd;
	
	/* Sequence number = position in the node section of the file */
	int _sequenceNumber;
	int _parentSequenceNumber; /* = -1 if this node is the root node */
	
	/* Where the Strings section begins (from the start of the node */
	int _stringSectionOffset;
	
	/* True if this node is closed (and to be committed to disk) */
	bool _isDone;
	
	/* Vector containing all the intervals contained in this node */
	std::vector<std::tr1::shared_ptr<Interval> > _intervals;

	int _nbChildren;		/* Nb. of children this node has */
	int* _children;		/* Seq. numbers of the children nodes (size = MAX_NB_CHILDREN) */
	uint64_t* _childStart;	/* Start times of each of the children (size = MAX_NB_CHILDREN) */
};

#endif // _HISTORYTREENODE_HPP
