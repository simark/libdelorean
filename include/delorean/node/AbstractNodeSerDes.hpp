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

// needed for cross-references
class Node;

/**
 * Base class for all node serializer/deserializer (ser/des).
 *
 * The job of a node ser/des is to serialize a complete node (all important
 * informations, children pointers and intervals) to a byte buffer and do
 * the exact opposite.
 *
 * The node ser/des also provides methods to get sizes of different parts of
 * the node since nodes track their current total size to be able to accept
 * or refuse new intervals.
 *
 * @author Philippe Proulx
 */
class AbstractNodeSerDes
{
public:
    /// Shared pointer to a node ser/des
    typedef std::shared_ptr<AbstractNodeSerDes> SP;

    /// Unique pointer to a node ser/des
    typedef std::unique_ptr<AbstractNodeSerDes> UP;

public:
    /**
     * Builds a node ser/des.
     */
    AbstractNodeSerDes();

    virtual ~AbstractNodeSerDes() = 0;

    /**
     * Serializes the node \p node beginning at the address \p headPtr. The
     * caller must make sure that enough memory is allocated at \p headPtr
     * to fit a whole node.
     *
     * @param node    Node to serialize
     * @param headPtr Address at which to write serialized data
     */
    virtual void serializeNode(const Node& node,
                               std::uint8_t* headPtr) const = 0;

    /**
     * Returns the header size of a node in bytes, according to the specific
     * way of serializing/deserializing implemented by a concrete node ser/des.
     *
     * @param node Node
     * @returns    Size of the node header
     */
    virtual std::size_t getHeaderSize(const Node& node) const = 0;

    /**
     * Returns the size of a child node pointer, according to the specific way
     * of serializing/deserializing implemented by a concrete node ser/des.
     *
     * @param cnp Child node pointer
     * @returns   Size of the child node pointer
     */
    virtual std::size_t getChildNodePointerSize(const ChildNodePointer& cnp) const = 0;

    /**
     * Returns the total size of an interval (header, fixed value and variable
     * data), according to the specific way of serializing/deserializing
     * implemented by a concrete node ser/des.
     *
     * @param interval Interval
     * @returns        Interval total size
     */
    virtual std::size_t getIntervalSize(const AbstractInterval& interval) const = 0;

    /**
     * Deserializes a node.
     *
     * @param headPtr     Address at which to read a node
     * @param size        Expected node size
     * @param maxChildren Expected maximum number of children
     * @returns           Deserialized node
     */
    virtual std::unique_ptr<Node> deserializeNode(const std::uint8_t* headPtr,
                                                  std::size_t size,
                                                  std::size_t maxChildren) const = 0;

    /**
     * Registers a new interval factory \p factory used to create intervals of
     * type \p type.
     *
     * @param type    Numerical type of intervals to be created by the factory
     * @param factory Factory
     */
    void registerIntervalFactory(interval_type_t type,
                                 IIntervalFactory::UP factory);

    /**
     * Registers a new interval factory \p factory used to create standard intervals
     * of type \p type.
     *
     * @param type    Numerical type of intervals to be created by the factory
     * @param factory Factory
     */
    void registerIntervalFactory(StandardIntervalType type,
                                 IIntervalFactory::UP factory);

    /**
     * Unregisters an interval factory already registered with
     * registerIntervalFactory().
     *
     * @param type Type of interval associated with factory to unregister
     */
    void unregisterIntervalFactory(interval_type_t type);

protected:
    AbstractInterval::UP createInterval(timestamp_t begin, timestamp_t end,
                                        interval_cat_id_t catId,
                                        interval_type_t type) const;
    std::unique_ptr<Node> createNode(std::size_t size, std::size_t maxChildren,
                                     node_seq_t seqNumber, node_seq_t parentSeqNumber,
                                     timestamp_t begin) const;

private:
    std::vector<IIntervalFactory::UP> _intervalFactories;
};

#endif // _ABSTRACTNODESERDES_HPP
