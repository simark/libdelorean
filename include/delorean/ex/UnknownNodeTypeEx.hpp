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
#ifndef _UNKNOWNNODETYPEEX_HPP
#define _UNKNOWNNODETYPEEX_HPP

#include <string>
#include <stdexcept>

#include "../basic_types.h"

class UnknownNodeTypeEx : public std::runtime_error
{
private:
	static std::string getMsg(node_type_t type);
public:
	UnknownNodeTypeEx(const std::string& msg) : runtime_error(msg) { }
	UnknownNodeTypeEx(interval_type_t type) : runtime_error(getMsg(type)) { }
};


#endif // _UNKNOWNNODETYPEEX_HPP


