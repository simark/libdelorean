#include "HistoryTreeNode.hpp"
#include "HistoryTree.hpp"

using namespace std;
using namespace std::tr1;

HistoryTreeNode::HistoryTreeNode()
:_nbChildren(0)
{
}

HistoryTreeNode::~HistoryTreeNode()
{
}

HistoryTreeNode HistoryTreeNode::readNode(const HistoryTree& tree)
{
	return HistoryTreeNode();
}

void HistoryTreeNode::writeInfoFromNode(vector<shared_ptr<Interval> >& intervals, uint64_t timestamp) const
{
	int startIndex;

	if ( _intervals.size() == 0 ) { return; }
	startIndex = getStartIndexFor(timestamp);

	for ( unsigned int i = startIndex; i < _intervals.size(); i++ ) {
		/* Now we only have to compare the Start times, since we now
		 * the End times necessarily fit */
		if ( _intervals[i]->getStart() <= timestamp ) {
			intervals[_intervals[i]->getAttribute()] = _intervals[i];
		}
	}
	return;
}

int HistoryTreeNode::getStartIndexFor(uint64_t timestamp) const
{
	//FIXME Use a binary search algorithm to find the correct index
	return 0;
}
