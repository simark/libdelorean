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
#ifndef _ABSTRACTHISTORYTREE_HPP
#define _ABSTRACTHISTORYTREE_HPP

#include <vector>
#include <fstream>

#include "HistoryTreeConfig.hpp"
#include "HistoryTreeNode.hpp"
#include "Interval.hpp"

class AbstractHistoryTree
{
public:
	AbstractHistoryTree();
	AbstractHistoryTree(HistoryTreeConfig config);
	
	// TODO: template method design pattern for those?
	virtual void open(void) = 0;
	virtual void close(void) = 0;
	virtual ~AbstractHistoryTree() { }
	
	void setConfig(HistoryTreeConfig config) {
		this->_config = config;
	}
	HistoryTreeConfig getConfig(void) const {		
		return this->_config;
	}

protected:
	unsigned int getHeaderSize(void) const {
		return AbstractHistoryTree::HEADER_SIZE;
	}
	static const unsigned int HEADER_SIZE;
	HistoryTreeConfig _config;
	bool _opened;
	timestamp_t _end;
	unsigned int _node_count;
	std::vector<HistoryTreeNodeSharedPtr> _latest_branch;
	std::fstream _stream;
};

#endif // _ABSTRACTHISTORYTREE_HPP
