/**
 * Copyright (c) 2012 Philippe Proulx <philippe.proulx@polymtl.ca>
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
#ifndef _HISTORYTREECORENODE_HPP
#define _HISTORYTREECORENODE_HPP

#include <stdint.h>
#include <ostream>
#include <vector>

#include "HistoryTreeConfig.hpp"
#include "Interval.hpp"
#include "HistoryTreeNode.hpp"
#include "basic_types.h"

using namespace std;
using namespace std::tr1;

class HistoryTreeCoreNode : public HistoryTreeNode 
{
public:
	HistoryTreeCoreNode();
	HistoryTreeCoreNode(HistoryTreeConfig config, seq_number_t seqNumber, seq_number_t parentSeqNumber, timestamp_t start);
	~HistoryTreeCoreNode();
	void serializeSpecificHeader(uint8_t* buf) const;
	unsigned int getSpecificHeaderSize(void) const;
	void linkNewChild(HistoryTreeNodeSharedPtr childNode);
	seq_number_t getChild(unsigned int index) const {
		return _children[index];
	}
	timestamp_t getChildStart(int index) const {
		return _childStart[index];
	}
	unsigned int getNbChildren() const {
		return _nbChildren;
	}

private:
	// allocates/frees children data
	void initChildren(void);
	void finiChildren(void);

	// number of children
	unsigned int _nbChildren;
	
	// children (size => owner tree config's max. children)
	seq_number_t* _children;
	
	// start time of each child (size => owner tree config's max. children)
	timestamp_t* _childStart;
};

#endif // _HISTORYTREECORENODE_HPP
