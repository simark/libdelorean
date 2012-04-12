/**
 * Copyright (c) 2012 Philippe Proulx <philippe.proulx@polymtl.ca>
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
#ifndef _EXISTINGINTERVALTYPEEX_HPP
#define _EXISTINGINTERVALTYPEEX_HPP

#include <string>
#include <stdexcept>

#include "../basic_types.h"

class ExistingIntervalTypeEx : public std::runtime_error
{
public:
	ExistingIntervalTypeEx(const std::string& msg) : runtime_error(msg) { }
};

#endif // _EXISTINGINTERVALTYPEEX_HPP