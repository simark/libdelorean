/* Copyright (c) 2014 Philippe Proulx <eepp.ca>
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
#include <memory>
#include <sstream>

#include <delorean/node/AbstractNodeSerDes.hpp>
#include <delorean/interval/IIntervalFactory.hpp>
#include <delorean/interval/IntInterval.hpp>
#include <delorean/interval/UIntInterval.hpp>
#include <delorean/interval/NullInterval.hpp>
#include <delorean/interval/FloatInterval.hpp>
#include <delorean/interval/StringInterval.hpp>
#include <delorean/interval/SimpleIntervalFactory.hpp>
#include <delorean/interval/StandardIntervalType.hpp>
#include <delorean/node/Node.hpp>
#include <delorean/ex/UnknownIntervalType.hpp>
#include <delorean/ex/ExistingIntervalType.hpp>
#include <delorean/BasicTypes.hpp>

AbstractNodeSerDes::AbstractNodeSerDes()
{
    // reserve space in factories vector
    _intervalFactories.resize(1 << (sizeof(interval_type_t) * 8));

    // register basic types we know
    auto intFactoryPtr = new SimpleIntervalFactory<IntInterval> {};
    auto uintFactoryPtr = new SimpleIntervalFactory<UIntInterval> {};
    auto stringFactoryPtr = new SimpleIntervalFactory<StringInterval> {};
    auto floatFactoryPtr = new SimpleIntervalFactory<FloatInterval> {};
    auto nullFactoryPtr = new SimpleIntervalFactory<NullInterval> {};

    IIntervalFactory::UP intFactory {intFactoryPtr};
    IIntervalFactory::UP uintFactory {uintFactoryPtr};
    IIntervalFactory::UP stringFactory {stringFactoryPtr};
    IIntervalFactory::UP floatFactory {floatFactoryPtr};
    IIntervalFactory::UP nullFactory {nullFactoryPtr};

    this->registerIntervalFactory(StandardIntervalType::INT32, std::move(intFactory));
    this->registerIntervalFactory(StandardIntervalType::UINT32, std::move(uintFactory));
    this->registerIntervalFactory(StandardIntervalType::STRING, std::move(stringFactory));
    this->registerIntervalFactory(StandardIntervalType::FLOAT32, std::move(floatFactory));
    this->registerIntervalFactory(StandardIntervalType::TNULL, std::move(nullFactory));
}

AbstractNodeSerDes::~AbstractNodeSerDes()
{
}

void AbstractNodeSerDes::unregisterIntervalFactory(interval_type_t type)
{
    _intervalFactories.at(type) = nullptr;
}

void AbstractNodeSerDes::registerIntervalFactory(interval_type_t type,
                                                 IIntervalFactory::UP factory)
{
    if (_intervalFactories.at(type) != nullptr) {
        std::ostringstream oss;
        oss << "interval type " << type << " is already registered";
        throw ExistingIntervalType(oss.str());
    } else {
        _intervalFactories.at(type) = std::move(factory);
    }
}

void AbstractNodeSerDes::registerIntervalFactory(StandardIntervalType type,
                                                 IIntervalFactory::UP factory)
{
    this->registerIntervalFactory(static_cast<interval_type_t>(type),
                                  std::move(factory));
}

Node::UP AbstractNodeSerDes::createNode(std::size_t size,
                                        std::size_t maxChildren,
                                        node_seq_t seqNumber,
                                        node_seq_t parentSeqNumber,
                                        timestamp_t begin) const
{
    Node::UP node {new Node {
        size, maxChildren, seqNumber, parentSeqNumber, begin, this
    }};

    return node;
}

AbstractInterval::UP AbstractNodeSerDes::createInterval(timestamp_t begin,
                                                        timestamp_t end,
                                                        interval_key_t key,
                                                        interval_type_t type) const
{
    // make sure interval factory exists for this type
    const auto& factory = _intervalFactories.at(type);
    if (factory == nullptr) {
        throw UnknownIntervalType {type};
    }

    // create interval
    auto interval = factory->create(begin, end, key);

    return interval;
}
