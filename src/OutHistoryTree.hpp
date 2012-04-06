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
#include "intervals/AbstractInterval.hpp"
#include "AbstractNode.hpp"
#include "CoreNode.hpp"
#include "LeafNode.hpp"
#include "ex/TimeRangeEx.hpp"
#include "basic_types.h"

class OutHistoryTree : virtual public AbstractHistoryTree
{
public:
	OutHistoryTree();
	OutHistoryTree(HistoryTreeConfig config);
	virtual void open();
	virtual void close(void) {
		this->close(this->_end);
	}
	virtual void close(timestamp_t end);
	virtual void addInterval(AbstractInterval::SharedPtr interval) throw(TimeRangeEx);
	virtual OutHistoryTree& operator<<(AbstractInterval::SharedPtr interval) throw(TimeRangeEx);
	~OutHistoryTree();

protected:
	virtual void tryInsertAtNode(AbstractInterval::SharedPtr interval, unsigned int index);
	virtual void addSiblingNode(unsigned int index);
	virtual void initEmptyTree(void);
	virtual void addNewRootNode(void);
	void openStream(void);
	void closeStream(void);
	void serializeHeader(void);
	virtual void serializeNode(AbstractNode::SharedPtr node);
	void incNodeCount(timestamp_t new_start);

	virtual CoreNode::SharedPtr initNewCoreNode(seq_number_t parent_seq, timestamp_t start);
	virtual LeafNode::SharedPtr initNewLeafNode(seq_number_t parent_seq, timestamp_t start);

private:

};

#endif // _OUTHISTORYTREE_HPP
