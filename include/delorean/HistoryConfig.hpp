/**
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
#ifndef _HISTORYTREECONFIG_HPP
#define _HISTORYTREECONFIG_HPP

#include <string>

#include "BasicTypes.hpp"

class HistoryConfig
{
public:
	// TODO: encapsulate?
	std::string _stateFile;
	unsigned int _blockSize;
	unsigned int _maxChildren;
	timestamp_t _treeStart;
	
	HistoryConfig(std::string file) : _stateFile(file) {		
	}
	
	HistoryConfig(std::string newStateFile, int blockSize, int maxChildren,
			timestamp_t startTime) 
	:_stateFile(newStateFile), _blockSize(blockSize), _maxChildren(maxChildren), _treeStart(startTime)
	{
	}
	
	/**
	 * Version using default values for blocksize and maxchildren
	 * @param stateFileName
	 * @param startTime
	 */
	HistoryConfig(std::string newStateFile, timestamp_t startTime) 
	:_stateFile(newStateFile), _blockSize(64 * 1024), _maxChildren(50), _treeStart(startTime)
	{
	}

	/**
	 * Default constructor
	 */	
	HistoryConfig()
	:_stateFile("/dev/urandom"), _blockSize(4096), _maxChildren(50), _treeStart(0)
	{
	}
};

#endif // _HISTORYTREECONFIG_HPP
