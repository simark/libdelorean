#ifndef _HISTORYTREEBACKEND_HPP
#define _HISTORYTREEBACKEND_HPP

#include <vector>
#include <tr1/memory>
#include <stdint.h>

#include "HistoryTree.hpp"

class Interval;

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
	HistoryTreeBackend();
	virtual ~HistoryTreeBackend();
	void insertInterval(const Interval& interval);
	void finish(uint64_t timestamp);
	std::vector< std::tr1::shared_ptr<Interval> > query(uint64_t timestamp) const;
	std::tr1::shared_ptr<Interval> query(uint64_t timestamp, int key) const;
	
private:
	HistoryTree _historyTree;
	
	bool checkValidTime(uint64_t timestamp) const;
	
};

#endif // _HISTORYTREEBACKEND_HPP

