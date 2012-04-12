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

#include <rbrntrvll/LeafNode.hpp>
#include <rbrntrvll/AbstractNode.hpp>
#include <rbrntrvll/IntervalCreator.hpp>
#include <rbrntrvll/basic_types.h>
#include <rbrntrvll/fixed_config.h>

using namespace std;
using namespace std::tr1;

const unsigned int LeafNode::HEADER_SIZE = 0;

LeafNode::LeafNode(HistoryTreeConfig config)
: AbstractNode(config) {
}

LeafNode::LeafNode(HistoryTreeConfig config, seq_number_t seqNumber,
seq_number_t parentSeqNumber, timestamp_t start)
: AbstractNode(config, seqNumber, parentSeqNumber, start, NT_LEAF) {
}

std::string LeafNode::getInfos(void) const {
	return "";
}

void LeafNode::unserializeSpecificHeader(std::istream& is) {
}