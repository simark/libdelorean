/**
 * Copyright (c) 2012 François Rajotte <francois.rajotte@polymtl.ca>
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
#ifndef _HISTORYTREE_HPP
#define _HISTORYTREE_HPP

#include <fstream>

#include "InHistoryTree.hpp"
#include "OutHistoryTree.hpp"
#include "HistoryTreeConfig.hpp"

class HistoryTree : public InHistoryTree, public OutHistoryTree
{
public:
	HistoryTree();
	HistoryTree(HistoryTreeConfig config);
	virtual ~HistoryTree();
	
	virtual void open();
	virtual void open(OpenMode mode);
	virtual void close(timestamp_t end);
	virtual void close();
private:
};

#endif // _HISTORYTREE_HPP
