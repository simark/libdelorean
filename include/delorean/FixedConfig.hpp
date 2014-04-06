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
#ifndef _FIXED_CONFIG_HPP
#define _FIXED_CONFIG_HPP

/**
 * Simple interval types.
 */
#define SIT_INT32	0
#define SIT_STRING	1
#define SIT_UINT32	2
#define SIT_FLOAT32	3
#define SIT_NULL	-1

/**
 * Node types.
 */
#define NT_CORE		1
#define NT_LEAF		2

/**
 * History file magic number.
 */
#define HF_MAGIC_NUMBER		0x05ffa900
#define HF_MAJOR		3
#define HF_MINOR		0

#endif // _FIXED_CONFIG_HPP
