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
#include <algorithm>
#include <cassert>
#include <ostream>
#include <cstring>

#include <delorean/LeafNode.hpp>
#include <delorean/AbstractNode.hpp>
#include <delorean/IntervalCreator.hpp>
#include <delorean/BasicTypes.hpp>
#include <delorean/FixedConfig.hpp>

using namespace std;
using namespace std::tr1;

const unsigned int LeafNode::HEADER_SIZE = 0;

LeafNode::LeafNode(HistoryConfig config)
: AbstractNode(config) {
}

LeafNode::LeafNode(HistoryConfig config, seq_number_t seqNumber,
seq_number_t parentSeqNumber, timestamp_t start)
: AbstractNode(config, seqNumber, parentSeqNumber, start, NT_LEAF) {
}

std::string LeafNode::getInfos(void) const {
	return "";
}

void LeafNode::unserializeSpecificHeader(std::istream& is) {
}
