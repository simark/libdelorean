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
#ifndef _ABSTRACTNODESERDES_HPP
#define _ABSTRACTNODESERDES_HPP

#include <vector>
#include <cstdint>
#include <memory>

#include <delorean/interval/AbstractInterval.hpp>
#include <delorean/interval/IIntervalFactory.hpp>
#include <delorean/interval/StandardIntervalType.hpp>
#include <delorean/node/ChildNodePointer.hpp>
#include <delorean/BasicTypes.hpp>

class Node;

class AbstractNodeSerDes
{
public:
    typedef std::shared_ptr<AbstractNodeSerDes> SP;
    typedef std::weak_ptr<AbstractNodeSerDes> WP;
    typedef std::unique_ptr<AbstractNodeSerDes> UP;

public:
    AbstractNodeSerDes();
    virtual ~AbstractNodeSerDes() = 0;

    // serializer side
    virtual void serializeNode(const Node& node,
                               std::uint8_t* headPtr) const = 0;
    virtual std::size_t getHeaderSize(const Node& node) const = 0;
    virtual std::size_t getChildNodePointerSize(const ChildNodePointer& cnp) const = 0;
    virtual std::size_t getIntervalSize(const AbstractInterval& interval) const = 0;

    // deserializer side
    virtual std::unique_ptr<Node> deserializeNode(const std::uint8_t* headPtr,
                                     std::size_t size,
                                     unsigned int maxChildren) const = 0;
    void registerIntervalFactory(interval_type_t type,
                                 IIntervalFactory::UP factory);
    void registerIntervalFactory(StandardIntervalType type,
                                 IIntervalFactory::UP factory);
    void unregisterIntervalFactory(interval_type_t type);

protected:
    AbstractInterval::UP createInterval(timestamp_t begin, timestamp_t end,
                                        interval_id_t id,
                                        interval_type_t type) const;
    std::unique_ptr<Node> createNode(std::size_t size, std::size_t maxChildren,
                        node_seq_t seqNumber, node_seq_t parentSeqNumber,
                        timestamp_t begin) const;

private:
    std::vector<IIntervalFactory::UP> _intervalFactories;
};

#endif // _ABSTRACTNODESERDES_HPP
