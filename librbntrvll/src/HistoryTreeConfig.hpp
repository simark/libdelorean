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
#ifndef _HISTORYTREECONFIG_HPP
#define _HISTORYTREECONFIG_HPP

#include <string>

class HistoryTreeConfig
{
public:
	std::string _stateFile;
	int _blockSize;
	int _maxChildren;
	timestamp_t _treeStart;
	
	HistoryTreeConfig(std::string newStateFile, int blockSize, int maxChildren,
			timestamp_t startTime) 
	:_stateFile(newStateFile), _blockSize(blockSize), _maxChildren(maxChildren), _treeStart(startTime)
	{
	}
	
	/**
	 * Version using default values for blocksize and maxchildren
	 * @param stateFileName
	 * @param startTime
	 */
	HistoryTreeConfig(std::string newStateFile, timestamp_t startTime) 
	:_stateFile(newStateFile), _blockSize(64 * 1024), _maxChildren(50), _treeStart(startTime)
	{
	}

	/**
	 * Default constructor, creates an invalid configuration
	 */	
	HistoryTreeConfig()
	:_stateFile("~/tmp/file"), _blockSize(0), _maxChildren(0), _treeStart(0)
	{
	}
};

#endif // _HISTORYTREECONFIG_HPP
