/* Copyright (c) 2014 Philippe Proulx <eepp.ca>
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
#ifndef _IHISTORYSINK_HPP
#define _IHISTORYSINK_HPP

#include <delorean/AbstractHistory.hpp>
#include <delorean/interval/AbstractInterval.hpp>

/**
 * History sink interface.
 *
 * All history sinks must inherit this interface to be qualified as
 * legitimate history sinks.
 *
 * @author Philippe Proulx
 */
class IHistorySink
{
public:
    /**
     * Adds an interval to the history. Intervals must be provided in ascending
     * order of end timestamp.
     *
     * @param interval Shared pointer to interval to add
     */
    virtual void addInterval(AbstractInterval::SP interval) = 0;

    /**
     * Closes the history file with a specific timestamp. This timestamp must
     * be greater than or equal to the current end timestamp.
     *
     * @param endTs End timestamp to use for closing the history file
     */
    virtual void close(timestamp_t endTs) = 0;
};

#endif // _IHISTORYSINK_HPP
