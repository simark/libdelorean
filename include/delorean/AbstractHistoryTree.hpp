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
#ifndef _ABSTRACTHISTORYTREE_HPP
#define _ABSTRACTHISTORYTREE_HPP

#include <vector>
#include <fstream>

#include "HistoryTreeConfig.hpp"
#include "AbstractNode.hpp"
#include "intervals/AbstractInterval.hpp"
#include "basic_types.h"

class AbstractHistoryTree
{
public:
	enum OpenMode { TRUNCATE, APPEND };

	AbstractHistoryTree();
	AbstractHistoryTree(HistoryTreeConfig config);
	
	// TODO: template method design pattern for those?
	virtual void open() = 0;
	virtual void close(timestamp_t end) = 0;
	virtual void close() = 0;
	virtual ~AbstractHistoryTree();
	
	void setConfig(HistoryTreeConfig config) {
		if (!_opened)
			this->_config = config;
	}
	HistoryTreeConfig getConfig(void) const {		
		return this->_config;
	}	
	timestamp_t getStart() const {
		return _config._treeStart;
	}
	
	timestamp_t getEnd() const {
		return _end;
	}
	
	timestamp_t getNodeCount() const {
		return _node_count;
	}

protected:
	bool checkValidTime(timestamp_t timestamp) const;
	bool nodeHasChildren(AbstractNode::ConstSharedPtr node) const;
	std::streampos filePosFromSeq(seq_number_t seq) const {
		return (uint64_t)this->getHeaderSize() + (uint64_t)seq * (uint64_t)this->_config._blockSize;
	}
	unsigned int getHeaderSize(void) const {
		return AbstractHistoryTree::HEADER_SIZE;
	}
	static const unsigned int HEADER_SIZE;
	HistoryTreeConfig _config;
	bool _opened;
	timestamp_t _end;
	unsigned int _node_count;
	std::vector<AbstractNode::SharedPtr> _latest_branch;
	mutable std::fstream _stream;
};

#endif // _ABSTRACTHISTORYTREE_HPP