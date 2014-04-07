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
#include <delorean/interval/AbstractInterval.hpp>
#include <delorean/BasicTypes.hpp>

struct IntervalHeader
{
    // standard interval header fields
    timestamp_t begin;
    timestamp_t end;
    interval_type_id_t typeId;

    interval_type_t getType() const
    {
        auto type = (typeId >> 24) & 0xff;

        return static_cast<interval_type_t>(type);
    }

    interval_id_t getId() const
    {
        auto id = typeId & 0xffffff;

        return static_cast<interval_id_t>(id);
    }

    void setTypeIdFromInterval(const AbstractInterval& interval)
    {
        auto type = static_cast<interval_type_id_t>(interval.getType());
        auto id = static_cast<interval_type_id_t>(interval.getId());

        typeId = type << 24 | (id & 0xffffff);
    }

    enum {
        SIZE = (
            sizeof(begin) +
            sizeof(end) +
            sizeof(typeId)
        )
    };
};
