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
#include "HistoryTreeBackend.hpp"
#include "Interval.hpp"
#include "IntInterval.hpp"
#include "HistoryTree.hpp"

#include <vector>

HistoryTreeBackend::HistoryTreeBackend()
{
}

HistoryTreeBackend::~HistoryTreeBackend()
{
}

void HistoryTreeBackend::insertInterval(const Interval& interval)
{
}

void HistoryTreeBackend::finish(uint64_t timestamp)
{
}
	
std::vector< std::tr1::shared_ptr<Interval> > HistoryTreeBackend::query(uint64_t timestamp) const
{
	//FIXME : simple stub method...
	std::vector< std::tr1::shared_ptr<Interval> > vecInterval;
	for(int i = 0; i < 100; i++)
	{
		std::tr1::shared_ptr<Interval> interval(new IntInterval(0, 100, i, 100-i));
		vecInterval.push_back(interval);
	}
	return vecInterval;
}

std::tr1::shared_ptr<Interval> HistoryTreeBackend::query(uint64_t timestamp, int key) const
{
	//FIXME : simple stub method...
	if(timestamp >= 100)
		return std::tr1::shared_ptr<Interval>(new IntInterval(timestamp-100, timestamp+100, key, 100));
	else
		return std::tr1::shared_ptr<Interval>(new IntInterval(0, timestamp+100, key, 100));
}
