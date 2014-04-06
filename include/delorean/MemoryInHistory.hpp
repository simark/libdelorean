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
#ifndef _MEMORYINHISTORYTREE_HPP
#define _MEMORYINHISTORYTREE_HPP

#include <vector>
#include <fstream>

#include "AbstractHistory.hpp"
#include "HistoryConfig.hpp"
#include "InHistory.hpp"
#include "AbstractMemoryHistory.hpp"
#include "intervals/AbstractInterval.hpp"
#include "AbstractNode.hpp"
#include "CoreNode.hpp"
#include "LeafNode.hpp"
#include "IntervalCreator.hpp"
#include "ex/TimeRangeEx.hpp"
#include "ex/InvalidFormatEx.hpp"
#include "BasicTypes.hpp"

class MemoryInHistory : virtual public InHistory, virtual public AbstractMemoryHistory
{
public:
	MemoryInHistory();
	MemoryInHistory(HistoryConfig config);
	virtual void open();
	virtual void close(void) {
		this->close(-1);
	}
	virtual void close(timestamp_t end);
	
	virtual AbstractNode::SharedPtr selectNextChild(CoreNode::SharedPtr currentNode, timestamp_t timestamp) const;
	virtual std::vector<AbstractInterval::SharedPtr> query(timestamp_t timestamp) const;
	virtual std::multimap<attribute_t, AbstractInterval::SharedPtr> sparseQuery(timestamp_t timestamp) const;
	virtual AbstractInterval::SharedPtr query(timestamp_t timestamp, attribute_t key) const;
protected:
	virtual AbstractNode::SharedPtr createNodeFromStream() const;
	virtual AbstractNode::SharedPtr createNodeFromSeq(seq_number_t seq) const;
	virtual AbstractNode::SharedPtr fetchNodeFromLatestBranch(seq_number_t seq) const;
	
	virtual void loadNodes();

private:
};

#endif // _MEMORYINHISTORYTREE_HPP
