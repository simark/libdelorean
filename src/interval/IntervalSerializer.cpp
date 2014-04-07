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
#include <cstring>
#include <cstdint>
#include <sstream>

#include <delorean/interval/IntervalSerializer.hpp>
#include <delorean/BasicTypes.hpp>

#include "IntervalStructs.hpp"

void IntervalSerializer::serializeInterval(const AbstractInterval& interval,
                                           std::uint8_t* headPtr,
                                           std::uint8_t* varAtPtr,
                                           std::size_t varOffset) const
{
    // build header
    IntervalHeader header {
        .begin = interval.getBegin(),
        .end = interval.getEnd()
    };
    header.setTypeIdFromInterval(interval);

    // write header
    std::memcpy(headPtr, &header, sizeof(header));
    headPtr += IntervalHeader::SIZE;

    // serialize values
    interval.serializeValues(headPtr, varAtPtr);

    // override fixed value if this interval has a variable section
    if (interval.getVariableSize() > 0) {
        auto offset = static_cast<interval_value_t>(varOffset);
        std::memcpy(headPtr, &offset, sizeof(offset));
    }
}
