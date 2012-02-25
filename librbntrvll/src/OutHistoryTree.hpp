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
#ifndef _OUTHISTORYTREE_HPP
#define _OUTHISTORYTREE_HPP

#include <vector>

#include "AbstractHistoryTree.hpp"
#include "HistoryTreeConfig.hpp"
#include "Interval.hpp"
#include "HistoryTreeNode.hpp"
#include "HistoryTreeCoreNode.hpp"
#include "HistoryTreeLeafNode.hpp"
#include "ex/TimeRangeEx.hpp"
#include "basic_types.h"

class OutHistoryTree : virtual public AbstractHistoryTree
{
public:
	OutHistoryTree();
	OutHistoryTree(HistoryTreeConfig config);
	void open(void);
	void close(timestamp_t end);
	void addInterval(IntervalSharedPtr interval) throw(TimeRangeEx);
	~OutHistoryTree();

protected:
	

private:
	void tryInsertAtNode(IntervalSharedPtr interval, unsigned int index);
	void addSiblingNode(unsigned int index);
	void addNewRootNode(void);
	void openStream(void);
	void closeStream(void);
	void serializeHeader(void);
	void serializeNode(HistoryTreeNodeSharedPtr node);
	HistoryTreeCoreNodeSharedPtr initNewCoreNode(seq_number_t parent_seq, timestamp_t start);
	HistoryTreeLeafNodeSharedPtr initNewLeafNode(seq_number_t parent_seq, timestamp_t start);
};

#endif // _OUTHISTORYTREE_HPP
