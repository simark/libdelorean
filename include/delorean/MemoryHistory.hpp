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
#ifndef _MEMORYHISTORYTREE_HPP
#define _MEMORYHISTORYTREE_HPP

#include <fstream>

#include "History.hpp"
#include "MemoryInHistory.hpp"
#include "MemoryOutHistory.hpp"
#include "HistoryConfig.hpp"

class MemoryHistory : public History, public MemoryInHistory, public MemoryOutHistory
{
public:
    MemoryHistory(bool writeOnClose = true);
    MemoryHistory(HistoryConfig config, bool writeOnClose = true);
    virtual ~MemoryHistory();

    virtual void open();
    virtual void open(OpenMode mode);
    virtual void close(timestamp_t end);
    virtual void close();

    virtual std::vector<AbstractInterval::SharedPtr> query(timestamp_t timestamp) const;
    virtual std::multimap<attribute_t, AbstractInterval::SharedPtr> sparseQuery(timestamp_t timestamp) const;
    virtual AbstractInterval::SharedPtr query(timestamp_t timestamp, attribute_t key) const;

    virtual void addInterval(AbstractInterval::SharedPtr interval) throw(TimeRangeEx);
private:
};

#endif // _MEMORYHISTORYTREE_HPP
