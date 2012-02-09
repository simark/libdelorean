#ifndef _HISTORYTREECONFIG_HPP
#define _HISTORYTREECONFIG_HPP

#include <string>

class HistoryTreeConfig
{
public:
	std::string _stateFile;
	int _blockSize;
	int _maxChildren;
	uint64_t _treeStart;
	
	HistoryTreeConfig(std::string newStateFile, int blockSize, int maxChildren,
			uint64_t startTime) 
	:_stateFile(newStateFile), _blockSize(blockSize), _maxChildren(maxChildren), _treeStart(startTime)
	{
	}
	
	/**
	 * Version using default values for blocksize and maxchildren
	 * @param stateFileName
	 * @param startTime
	 */
	HistoryTreeConfig(std::string newStateFile, uint64_t startTime) 
	:_stateFile(newStateFile), _blockSize(64 * 1024), _maxChildren(50), _treeStart(startTime)
	{
	}
	
	HistoryTreeConfig()
	:_stateFile("~/tmp/file"), _blockSize(0), _maxChildren(0), _treeStart(0)
	{
	}
};

#endif // _HISTORYTREECONFIG_HPP
