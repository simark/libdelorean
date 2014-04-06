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
#ifndef _MEMORYOUTHISTORYTREE_HPP
#define _MEMORYOUTHISTORYTREE_HPP

#include <vector>

#include "AbstractMemoryHistoryTree.hpp"
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

class MemoryOutHistoryTree : virtual public OutHistoryTree, virtual public AbstractMemoryHistoryTree
{
public:
	MemoryOutHistoryTree(bool writeOnClose = true);
	MemoryOutHistoryTree(HistoryTreeConfig config, bool writeOnClose = true);
	virtual void open();
	virtual void close(void) {
		this->close(this->_end);
	}
	virtual void close(timestamp_t end);
	virtual void setWriteOnClose(bool write) { _writeOnClose = write; };
	bool getWriteOnClose() { return _writeOnClose; };
	virtual ~MemoryOutHistoryTree();
	virtual void addInterval(AbstractInterval::SharedPtr interval) throw(TimeRangeEx);

protected:	
	virtual void addSiblingNode(unsigned int index);
	virtual void initEmptyTree(void);
	virtual void addNewRootNode(void);
	virtual CoreNode::SharedPtr initNewCoreNode(seq_number_t parent_seq, timestamp_t start);
	virtual LeafNode::SharedPtr initNewLeafNode(seq_number_t parent_seq, timestamp_t start);
	
	void manageInsert(void);
	void startThread(void);
	void stopThread(void);
	
	virtual void writeToFile();
	
	std::queue<AbstractInterval::SharedPtr> _insertQueue;
	boost::mutex _insertQueue_mutex;
	boost::thread _insertThread;
	boost::condition_variable _insertConditionVariable;
private:
	bool _writeOnClose;
};

#endif // _MEMORYOUTHISTORYTREE_HPP
