/**
 * Copyright (c) 2012 Philippe Proulx <philippe.proulx@polymtl.ca>
 *
 * This file is part of libdelorean.
 *
 * libdelorean is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libdelorean is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libdelorean.  If not, see <http://www.gnu.org/licenses/>.
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
	ThreadedOutHistoryTree(unsigned int maxQueueSize = 10000);
	ThreadedOutHistoryTree(HistoryTreeConfig config, unsigned int maxQueueSize = 10000);
	virtual void open();
	virtual void close(timestamp_t end);
	virtual void addInterval(AbstractInterval::SharedPtr interval) throw(TimeRangeEx);

protected:	
	virtual void addSiblingNode(unsigned int index);
	virtual void initEmptyTree(void);
	virtual void addNewRootNode(void);
	virtual void serializeNode(AbstractNode::SharedPtr node);
	
	void manageInsert(void);
	void startThread(void);
	void stopThread(void);
	
	std::queue<AbstractInterval::SharedPtr> _insertQueue;
	boost::mutex _insertQueue_mutex;
	const unsigned int _maxQueueSize;
	boost::thread _insertThread;
	boost::condition_variable _insertConditionVariable;
	boost::condition_variable _queueFullConditionVariable;
private:

};

#endif // _THREADEDOUTHISTORYTREE_HPP
