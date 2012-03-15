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
#ifndef _HISTORYTREEBACKEND_HPP
#define _HISTORYTREEBACKEND_HPP

#include <vector>
#include <tr1/memory>
#include <stdint.h>

#include "HistoryTree.hpp"
#include "intervals/Interval.hpp"

class HistoryTreeIO;

/**
 * @class HistoryTreeBackend
 * 
 * The history tree backend is the entry point of the Interval library.
 * It contains all the high level methods to use the history tree
 * 
 */
class HistoryTreeBackend
{
public:
	HistoryTreeBackend(std::string newFile, int blockSize, int maxChildren, timestamp_t startTime);
	HistoryTreeBackend(std::string newFile, timestamp_t startTime);
	HistoryTreeBackend(std::string existingFile);
	virtual ~HistoryTreeBackend();
	void insertInterval(IntervalSharedPtr interval);
	void finish(timestamp_t timestamp);
	std::vector< IntervalSharedPtr > query(timestamp_t timestamp) const;
	IntervalSharedPtr query(timestamp_t timestamp, attribute_t key) const;
	
private:
	HistoryTree _historyTree;
	HistoryTreeIO _treeIO;
	
	bool checkValidTime(timestamp_t timestamp) const;
	IntervalSharedPtr getRelevantInterval(timestamp_t timestamp, attribute_t key) const;
	
};

#endif // _HISTORYTREEBACKEND_HPP

