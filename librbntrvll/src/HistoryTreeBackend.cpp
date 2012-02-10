#include "HistoryTreeBackend.hpp"
#include "Interval.hpp"
#include "IntInterval.hpp"
#include "HistoryTree.hpp"
#include "HistoryTreeNode.hpp"

#include <vector>

using namespace std;
using namespace std::tr1;

HistoryTreeBackend::HistoryTreeBackend()
{
}

HistoryTreeBackend::~HistoryTreeBackend()
{
}

void HistoryTreeBackend::insertInterval(const Interval& interval)
{
	_historyTree.insertInterval(interval);
}

void HistoryTreeBackend::finish(uint64_t timestamp)
{
}
	
vector<shared_ptr<Interval> > HistoryTreeBackend::query(uint64_t timestamp) const
{
	//FIXME : simple stub method...
	//vector<shared_ptr<Interval> > vecInterval;
	//for(int i = 0; i < 100; i++)
	//{
		//shared_ptr<Interval> interval(new IntInterval(0, 100, i, 100-i));
		//vecInterval.push_back(interval);
	//}
	//return vecInterval;
	
	if ( !checkValidTime(timestamp) ) {
		//FIXME : Throw an exception here
	}
	
	/* We start by reading the information in the root node */
	HistoryTreeNode currentNode = _historyTree.getLatestBranch()[0];
	vector<shared_ptr<Interval> > relevantIntervals;
	currentNode.writeInfoFromNode(relevantIntervals, timestamp);
	
	/* Then we follow the branch down in the relevant children */
	while ( currentNode.getNbChildren() > 0 ) {
		currentNode = _historyTree.selectNextChild(currentNode, timestamp);
		currentNode.writeInfoFromNode(relevantIntervals, timestamp);
	}
	
	/* The relevantIntervals should now be filled with everything needed */
	return relevantIntervals;	
}

std::tr1::shared_ptr<Interval> HistoryTreeBackend::query(uint64_t timestamp, int key) const
{
	//FIXME : simple stub method...
	if(timestamp >= 100)
		return std::tr1::shared_ptr<Interval>(new IntInterval(timestamp-100, timestamp+100, key, 100));
	else
		return std::tr1::shared_ptr<Interval>(new IntInterval(0, timestamp+100, key, 100));
}

bool HistoryTreeBackend::checkValidTime(uint64_t timestamp) const
{
	return ( timestamp >= _historyTree.getTreeStart() && timestamp <= _historyTree.getTreeEnd() );
}
