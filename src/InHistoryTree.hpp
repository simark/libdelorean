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
#ifndef _INHISTORYTREE_HPP
#define _INHISTORYTREE_HPP

#include <vector>
#include <fstream>

#include "AbstractHistoryTree.hpp"
#include "HistoryTreeConfig.hpp"
#include "intervals/AbstractInterval.hpp"
#include "AbstractNode.hpp"
#include "CoreNode.hpp"
#include "LeafNode.hpp"
#include "IntervalCreator.hpp"
#include "ex/TimeRangeEx.hpp"
#include "ex/InvalidFormatEx.hpp"
#include "basic_types.h"

class InHistoryTree : virtual public AbstractHistoryTree
{
public:
	InHistoryTree();
	InHistoryTree(HistoryTreeConfig config);
	virtual void open();
	virtual void close(void) {
		this->close(-1);
	}
	virtual void close(timestamp_t end);
	IntervalCreator& getIC(void) {
		return _ic;
	}
	virtual AbstractNode::SharedPtr selectNextChild(CoreNode::SharedPtr currentNode, timestamp_t timestamp) const;
	virtual std::vector<AbstractInterval::SharedPtr> query(timestamp_t timestamp) const;
	virtual std::multimap<attribute_t, AbstractInterval::SharedPtr> sparseQuery(timestamp_t timestamp) const;
	virtual AbstractInterval::SharedPtr query(timestamp_t timestamp, attribute_t key) const;
	virtual void test(void);
	~InHistoryTree();

protected:
	virtual void buildLatestBranch(void);
	virtual void unserializeHeader(void);
	
	virtual AbstractNode::SharedPtr createNodeFromStream() const;
	virtual AbstractNode::SharedPtr createNodeFromSeq(seq_number_t seq) const;
	virtual AbstractNode::SharedPtr fetchNodeFromLatestBranch(seq_number_t seq) const;
	
	seq_number_t _root_seq;
	IntervalCreator _ic;
private:
};

#endif // _INHISTORYTREE_HPP
