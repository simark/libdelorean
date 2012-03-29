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
#include "AbstractHistoryTree.hpp"
#include "HistoryTreeCoreNode.hpp"

using namespace std;
using namespace std::tr1;
 
const unsigned int AbstractHistoryTree::HEADER_SIZE = 4096;

AbstractHistoryTree::AbstractHistoryTree()
: _opened(false) {
}

AbstractHistoryTree::AbstractHistoryTree(HistoryTreeConfig config) :
_config(config), _opened(false) {
}

AbstractHistoryTree::~AbstractHistoryTree() {
}


bool AbstractHistoryTree::checkValidTime(timestamp_t timestamp) const {	
	return ( timestamp >= _config._treeStart && timestamp <= _end );
}

bool AbstractHistoryTree::nodeHasChildren(ConstHistoryTreeNodeSharedPtr node) const {
	ConstHistoryTreeCoreNodeSharedPtr coreNode = dynamic_pointer_cast<const HistoryTreeCoreNode>(node);
	return(coreNode && coreNode->getNbChildren());
}