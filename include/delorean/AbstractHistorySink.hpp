/**
 * Copyright (c) 2014 Philippe Proulx <eepp.ca>
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
#ifndef _ABSTRACTHISTORYSINK_HPP
#define _ABSTRACTHISTORYSINK_HPP

#include <delorean/AbstractHistory.hpp>
#include <delorean/interval/AbstractInterval.hpp>

class AbstractHistorySink :
    public AbstractHistory
{
public:
    virtual void addInterval(AbstractInterval::SP interval) = 0;
    virtual void close(timestamp_t end) = 0;
};

#endif // _ABSTRACTHISTORYSINK_HPP
