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
#ifndef _HISTORYTREELEAFNODE_HPP
#define _HISTORYTREELEAFNODE_HPP

#include <stdint.h>
#include <ostream>
#include <vector>
#include <tr1/memory>

#include "HistoryTreeConfig.hpp"
#include "Interval.hpp"
#include "IntervalCreator.hpp"
#include "HistoryTreeNode.hpp"
#include "basic_types.h"

class HistoryTreeLeafNode;

typedef std::tr1::shared_ptr<HistoryTreeLeafNode> HistoryTreeLeafNodeSharedPtr;

class HistoryTreeLeafNode : public HistoryTreeNode 
{
public:
	HistoryTreeLeafNode(HistoryTreeConfig config);
	HistoryTreeLeafNode(HistoryTreeConfig config, seq_number_t seqNumber, seq_number_t parentSeqNumber, timestamp_t start);
	~HistoryTreeLeafNode() { }
	std::string getInfos(void) const;
	void serializeSpecificHeader(uint8_t* buf) const {
		// no header here!
	}
	void unserializeSpecificHeader(std::istream& is);
	unsigned int getSpecificHeaderSize(void) const {
		return HistoryTreeLeafNode::HEADER_SIZE;
	}
	static const unsigned int HEADER_SIZE;

private:
};

#endif // _HISTORYTREELEAFNODE_HPP
