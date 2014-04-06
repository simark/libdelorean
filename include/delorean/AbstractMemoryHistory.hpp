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
#ifndef _ABSTRACTMEMORYHISTORYTREE_HPP
#define _ABSTRACTMEMORYHISTORYTREE_HPP

#include "AbstractHistory.hpp"
#include <boost/thread/mutex.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/shared_mutex.hpp>

class AbstractMemoryHistory : virtual public AbstractHistory
{
public:
	AbstractMemoryHistory();
	AbstractMemoryHistory(HistoryConfig config);
	
protected:
	mutable boost::shared_mutex _latest_branch_mutex;
	
	mutable boost::shared_mutex _nodes_mutex;
	std::vector<AbstractNode::SharedPtr> _nodes;
};

#endif // _ABSTRACTMEMORYINHISTORYTREE_HPP
