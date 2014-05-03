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
#ifndef _UNKNOWNNODESERDESTYPE_HPP
#define _UNKNOWNNODESERDESTYPE_HPP

#include <string>
#include <stdexcept>

#include <delorean/node/NodeSerDesType.hpp>

namespace delo
{
namespace ex
{

class UnknownNodeSerDesType :
    public std::runtime_error
{
public:
    UnknownNodeSerDesType(NodeSerDesType type) :
        std::runtime_error {"unknown node serializer/deserializer type"},
        _type {type}
    {
    }

    NodeSerDesType getType() const {
        return _type;
    }

private:
    NodeSerDesType _type;
};

}
}

#endif // _UNKNOWNNODESERDESTYPE_HPP
