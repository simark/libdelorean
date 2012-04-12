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
#ifndef _THREADEDHISTORYTREE_HPP
#define _THREADEDHISTORYTREE_HPP

#include <fstream>

#include "HistoryTree.hpp"
#include "ThreadedInHistoryTree.hpp"
#include "ThreadedOutHistoryTree.hpp"
#include "HistoryTreeConfig.hpp"

class ThreadedHistoryTree : public HistoryTree, public ThreadedInHistoryTree, public ThreadedOutHistoryTree 
{
public:
	ThreadedHistoryTree();
	ThreadedHistoryTree(HistoryTreeConfig config);
	virtual ~ThreadedHistoryTree();
	
	virtual void open();
	virtual void open(OpenMode mode);
	virtual void close(timestamp_t end);
	virtual void close();
	
	virtual std::vector<AbstractInterval::SharedPtr> query(timestamp_t timestamp) const;
	virtual std::multimap<attribute_t, AbstractInterval::SharedPtr> sparseQuery(timestamp_t timestamp) const;
	virtual AbstractInterval::SharedPtr query(timestamp_t timestamp, attribute_t key) const;
	
	virtual void addInterval(AbstractInterval::SharedPtr interval) throw(TimeRangeEx);
private:
};

#endif // _THREADEDHISTORYTREE_HPP
