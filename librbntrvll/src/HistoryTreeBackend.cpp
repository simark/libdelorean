/**
 * Copyright (c) 2012 François Rajotte <francois.rajotte@polymtl.ca>
 *
 * This file is part of librbntrvll.
 *
 * librbntrvll is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * librbntrvll is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with librbntrvll.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "HistoryTreeBackend.hpp"
#include "Interval.hpp"
#include "IntInterval.hpp"
#include "HistoryTree.hpp"
#include "HistoryTreeNode.hpp"
#include "ex/TimeRangeEx.hpp"

#include <vector>
#include <assert.h>

using namespace std;
using namespace std::tr1;

HistoryTreeBackend::HistoryTreeBackend()
{
}

HistoryTreeBackend::~HistoryTreeBackend()
{
}

/**
 * Adds a new interval to the history tree
 * 
 * @param interval A new interval to insert
 */
void HistoryTreeBackend::insertInterval(const Interval& interval)
{
	_historyTree.insertInterval(interval);
}

void HistoryTreeBackend::finish(timestamp_t timestamp)
{
	_historyTree.closeTree(timestamp);
}
	
vector<shared_ptr<Interval> > HistoryTreeBackend::query(timestamp_t timestamp) const
{
	
	if ( !checkValidTime(timestamp) ) {
		throw TimeRangeEx("Query timestamp outside of TreeInterval bounds.");
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

std::tr1::shared_ptr<Interval> HistoryTreeBackend::query(timestamp_t timestamp, attribute_t key) const
{
	//FIXME : simple stub method...
	if(timestamp >= 100)
		return std::tr1::shared_ptr<Interval>(new IntInterval(timestamp-100, timestamp+100, key, 100));
	else
		return std::tr1::shared_ptr<Interval>(new IntInterval(0, timestamp+100, key, 100));
}

bool HistoryTreeBackend::checkValidTime(timestamp_t timestamp) const
{
	return ( timestamp >= _historyTree.getTreeStart() && timestamp <= _historyTree.getTreeEnd() );
}

std::tr1::shared_ptr<Interval> HistoryTreeBackend::getRelevantInterval(timestamp_t timestamp, attribute_t key) const
{
	if ( !checkValidTime(timestamp) ) {
		throw TimeRangeEx("Query timestamp outside of TreeInterval bounds.");
	}
	
	HistoryTreeNode currentNode = _historyTree.getLatestBranch()[0];
	shared_ptr<Interval> interval = currentNode.getRelevantInterval(timestamp, key);
	
	while ( interval == NULL && currentNode.getNbChildren() > 0 ) {
		currentNode = _historyTree.selectNextChild(currentNode, timestamp);
		interval = currentNode.getRelevantInterval(timestamp, key);
	}
	/* Since we should now have intervals at every attribute/timestamp
	 * combination, it should NOT be null here. */
	assert (interval != NULL);
	return interval;
}
