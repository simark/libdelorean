/**
 * Copyright (c) 2012 Philippe Proulx <philippe.proulx@polymtl.ca>
 * Copyright (c) 2012 François Rajotte <francois.rajotte@polymtl.ca>
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
#ifndef _CORENODE_HPP
#define _CORENODE_HPP

#include <stdint.h>
#include <ostream>
#include <vector>
#include <tr1/memory>

#include "HistoryConfig.hpp"
#include "intervals/AbstractInterval.hpp"
#include "IntervalCreator.hpp"
#include "AbstractNode.hpp"
#include "BasicTypes.hpp"

class CoreNode : public AbstractNode 
{
public:
	typedef std::tr1::shared_ptr<CoreNode> SharedPtr;
	typedef std::tr1::shared_ptr<const CoreNode> ConstSharedPtr;

	CoreNode(HistoryConfig config);
	CoreNode(HistoryConfig config, seq_number_t seqNumber, seq_number_t parentSeqNumber, timestamp_t start);
	~CoreNode();
	std::string getInfos(void) const;
	void serializeSpecificHeader(uint8_t* buf) const;
	void unserializeSpecificHeader(std::istream& is);
	unsigned int getSpecificHeaderSize(void) const;
	void linkNewChild(AbstractNode::SharedPtr childNode);
	seq_number_t getChildAtTimestamp(timestamp_t timestamp) const;
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

#endif // _CORENODE_HPP
