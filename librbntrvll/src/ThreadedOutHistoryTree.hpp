/**
 * Copyright (c) 2012 Philippe Proulx <philippe.proulx@polymtl.ca>
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
#ifndef _THREADEDOUTHISTORYTREE_HPP
#define _THREADEDOUTHISTORYTREE_HPP

#include <vector>

#include "AbstractThreadedHistoryTree.hpp"
#include "OutHistoryTree.hpp"
#include "HistoryTreeConfig.hpp"
#include "intervals/AbstractInterval.hpp"
#include "AbstractNode.hpp"
#include "CoreNode.hpp"
#include "LeafNode.hpp"
#include "ex/TimeRangeEx.hpp"
#include "basic_types.h"

#include <queue>
#include <boost/thread/thread.hpp>

class ThreadedOutHistoryTree : virtual public OutHistoryTree, virtual public AbstractThreadedHistoryTree
{
public:
	ThreadedOutHistoryTree();
	ThreadedOutHistoryTree(HistoryTreeConfig config);
	virtual void open();
	virtual void close(timestamp_t end);
	virtual void addInterval(AbstractInterval::SharedPtr interval) throw(TimeRangeEx);

protected:	
	void addSiblingNode(unsigned int index);
	void initEmptyTree(void);
	void addNewRootNode(void);
	
	void manageInsert(void);
	void startThread(void);
	void stopThread(void);
	
	std::queue<AbstractInterval::SharedPtr> _insertQueue;
	boost::mutex _insertQueue_mutex;
	boost::thread _insertThread;
	boost::condition_variable _insertConditionVariable;
private:

};

#endif // _THREADEDOUTHISTORYTREE_HPP
