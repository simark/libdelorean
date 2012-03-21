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
#include "intervals/Interval.hpp"
#include "HistoryTreeNode.hpp"
#include "HistoryTreeCoreNode.hpp"
#include "HistoryTreeLeafNode.hpp"
#include "IntervalCreator.hpp"
#include "ex/TimeRangeEx.hpp"
#include "ex/InvalidFormatEx.hpp"
#include "basic_types.h"

class InHistoryTree : virtual public AbstractHistoryTree
{
public:
	InHistoryTree();
	InHistoryTree(HistoryTreeConfig config);
	void open();
	void close(void) {
		this->close(-1);
	}
	void close(timestamp_t end);
	IntervalCreator& getIC(void) {
		return _ic;
	}
	HistoryTreeNodeSharedPtr selectNextChild(HistoryTreeCoreNodeSharedPtr currentNode, timestamp_t timestamp) const;
	std::vector< IntervalSharedPtr > query(timestamp_t timestamp) const;
	IntervalSharedPtr query(timestamp_t timestamp, attribute_t key) const;
	void test(void);
	~InHistoryTree();

protected:
	void buildLatestBranch(void);
	void unserializeHeader(void);

private:
	HistoryTreeNodeSharedPtr createNodeFromStream() const;
	HistoryTreeNodeSharedPtr createNodeFromSeq(seq_number_t seq) const;
	HistoryTreeNodeSharedPtr fetchNodeFromLatestBranch(seq_number_t seq) const;
	seq_number_t _root_seq;
	IntervalCreator _ic;
};

#endif // _INHISTORYTREE_HPP
