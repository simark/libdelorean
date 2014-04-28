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
#ifndef _IHISTORYSOURCE_HPP
#define _IHISTORYSOURCE_HPP

#include <vector>

#include <delorean/AbstractHistory.hpp>
#include <delorean/interval/IntervalJar.hpp>
#include <delorean/interval/AbstractInterval.hpp>
#include <delorean/BasicTypes.hpp>

/**
 * History source interface.
 *
 * All history sources must inherit this interface to be qualified as
 * legitimate history sources.
 *
 * @author Philippe Proulx
 */
class IHistorySource
{
public:
    /**
     * Finds all intervals in the history intersecting \p ts and adds them
     * to \p intervals.
     *
     * @param ts        Timestamp
     * @param intervals Jar of intervals in which to add matching intervals
     * @returns         True if at least one interval was found
     */
    virtual bool findAll(timestamp_t ts, IntervalJar& intervals) = 0;

    /**
     * Finds a single interval in the history intersecting \p ts and matching
     * key \p key.
     *
     * @param ts    Timestamp
     * @param key   Key
     * @returns     Shared pointer to matching interval or \a nullptr
     */
    virtual AbstractInterval::SP findOne(timestamp_t ts,
                                         interval_key_t key) = 0;
};

#endif // _IHISTORYSOURCE_HPP
