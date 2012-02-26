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
#include <algorithm>
#include <cassert>
#include <ostream>
#include <cstring>

#include "HistoryTreeLeafNode.hpp"
#include "HistoryTreeNode.hpp"
#include "IntervalCreator.hpp"
#include "basic_types.h"
#include "fixed_config.h"

using namespace std;
using namespace std::tr1;

const unsigned int HistoryTreeLeafNode::HEADER_SIZE = 0;

HistoryTreeLeafNode::HistoryTreeLeafNode(HistoryTreeConfig config)
: HistoryTreeNode(config) {
}

HistoryTreeLeafNode::HistoryTreeLeafNode(HistoryTreeConfig config, seq_number_t seqNumber,
seq_number_t parentSeqNumber, timestamp_t start)
: HistoryTreeNode(config, seqNumber, parentSeqNumber, start, NT_LEAF) {
}

std::string HistoryTreeLeafNode::getInfos(void) const {
	return "";
}

void HistoryTreeLeafNode::unserializeSpecificHeader(std::istream& is) {
}
