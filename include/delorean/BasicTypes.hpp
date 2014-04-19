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
#ifndef _BASIC_TYPES_HPP
#define _BASIC_TYPES_HPP

#include <cstdint>

/**
 * @file
 * This header holds basic type definitions used throughout libdelorean.
 */

/// Timestamp
typedef std::uint64_t       timestamp_t;

/// Interval reference ID
typedef std::uint32_t       interval_id_t;

/// Interval type
typedef std::uint8_t        interval_type_t;

/// Interval fixed 32-bit value
typedef std::uint32_t       interval_value_t;

/// Node sequence number
typedef std::uint32_t       node_seq_t;

#endif // _BASIC_TYPES_HPP
