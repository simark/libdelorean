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
#ifndef _LEAFNODE_HPP
#define _LEAFNODE_HPP

#include <stdint.h>
#include <ostream>
#include <vector>
#include <tr1/memory>

#include "HistoryConfig.hpp"
#include "intervals/AbstractInterval.hpp"
#include "IntervalCreator.hpp"
#include "AbstractNode.hpp"
#include "BasicTypes.hpp"

class LeafNode : public AbstractNode 
{
public:
	typedef std::tr1::shared_ptr<LeafNode> SharedPtr;
	typedef std::tr1::shared_ptr<const LeafNode> ConstSharedPtr;

	LeafNode(HistoryConfig config);
	LeafNode(HistoryConfig config, seq_number_t seqNumber, seq_number_t parentSeqNumber, timestamp_t start);
	~LeafNode() { }
	std::string getInfos(void) const;
	void serializeSpecificHeader(uint8_t* buf) const {
		// no header here!
	}
	void unserializeSpecificHeader(std::istream& is);
	unsigned int getSpecificHeaderSize(void) const {
		return LeafNode::HEADER_SIZE;
	}
	static const unsigned int HEADER_SIZE;

private:
};

#endif // _LEAFNODE_HPP
