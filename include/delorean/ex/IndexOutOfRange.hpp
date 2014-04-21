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
#ifndef _INDEXOUTOFRANGE_HPP
#define _INDEXOUTOFRANGE_HPP

#include <stdexcept>
#include <cstddef>

#include <delorean/BasicTypes.hpp>

class IndexOutOfRange :
    public std::out_of_range
{
public:
    IndexOutOfRange(std::size_t size, std::size_t index) :
        std::out_of_range {"Index out of range"},
        _size {size},
        _index {index}
    {
    }

    std::size_t getSize() const
    {
        return _size;
    }

    std::size_t getIndex() const
    {
        return _index;
    }

private:
    std::size_t _size;
    std::size_t _index;
};

#endif // _INDEXOUTOFRANGE_HPP
