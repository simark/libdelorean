#include "HistoryTreeBackend.hpp"
#include "Interval.hpp"
#include "IntInterval.hpp"
#include "HistoryTree.hpp"

#include <vector>

HistoryTreeBackend::HistoryTreeBackend()
{
}

HistoryTreeBackend::~HistoryTreeBackend()
{
}

void HistoryTreeBackend::insertInterval(const Interval& interval)
{
}

void HistoryTreeBackend::finish(uint64_t timestamp)
{
}
	
std::vector< std::tr1::shared_ptr<Interval> > HistoryTreeBackend::query(uint64_t timestamp) const
{
	//FIXME : simple stub method...
	std::vector< std::tr1::shared_ptr<Interval> > vecInterval;
	for(int i = 0; i < 100; i++)
	{
		std::tr1::shared_ptr<Interval> interval(new IntInterval(0, 100, i, 100-i));
		vecInterval.push_back(interval);
	}
	return vecInterval;
}

std::tr1::shared_ptr<Interval> HistoryTreeBackend::query(uint64_t timestamp, int key) const
{
	//FIXME : simple stub method...
	if(timestamp >= 100)
		return std::tr1::shared_ptr<Interval>(new IntInterval(timestamp-100, timestamp+100, key, 100));
	else
		return std::tr1::shared_ptr<Interval>(new IntInterval(0, timestamp+100, key, 100));
}