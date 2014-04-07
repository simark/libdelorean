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
#include <cstdlib>
#include <cstdint>
#include <sstream>
#include <cstring>
#include <algorithm>

#include <delorean/interval/IntervalDeserializer.hpp>
#include <delorean/interval/IntInterval.hpp>
#include <delorean/interval/UIntInterval.hpp>
#include <delorean/interval/NullInterval.hpp>
#include <delorean/interval/FloatInterval.hpp>
#include <delorean/interval/StringInterval.hpp>
#include <delorean/interval/IIntervalFactory.hpp>
#include <delorean/interval/SimpleIntervalFactory.hpp>
#include <delorean/interval/StandardIntervalTypes.hpp>
#include <delorean/BasicTypes.hpp>
#include <delorean/ex/UnknownIntervalType.hpp>
#include <delorean/ex/ExistingIntervalType.hpp>

#include "IntervalStructs.hpp"

IntervalDeserializer::IntervalDeserializer()
{
    // reserve space in factories vector
    _factories.resize(1 << (sizeof(interval_type_t) * 8));

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

    this->registerIntervalType(StandardIntervalTypes::INT32, std::move(intFactory));
    this->registerIntervalType(StandardIntervalTypes::UINT32, std::move(uintFactory));
    this->registerIntervalType(StandardIntervalTypes::STRING, std::move(stringFactory));
    this->registerIntervalType(StandardIntervalTypes::FLOAT32, std::move(floatFactory));
    this->registerIntervalType(StandardIntervalTypes::TNULL, std::move(nullFactory));
}

IntervalDeserializer::~IntervalDeserializer()
{
}

void IntervalDeserializer::unregisterIntervalType(interval_type_t type)
{
    if (_factories[type] != nullptr) {
        _factories[type] = nullptr;
    }
}

void IntervalDeserializer::registerIntervalType(interval_type_t type,
                                                std::unique_ptr<IIntervalFactory> factory)
{
    if (_factories[type] != nullptr) {
        std::ostringstream oss;
        oss << "interval type " << type << " is already registered";
        throw ExistingIntervalType(oss.str());
    } else {
        _factories[type] = std::move(factory);
    }
}

void IntervalDeserializer::registerIntervalType(StandardIntervalTypes type,
                                                IIntervalFactory::UP factory)
{
    auto typeGood = static_cast<interval_type_t>(type);

    this->registerIntervalType(typeGood, std::move(factory));
}

void IntervalDeserializer::unregisterAll()
{
    for (unsigned int i = 0; i < _factories.size(); ++i) {
        this->unregisterIntervalType(i);
    }
}

AbstractInterval::UP IntervalDeserializer::deserializeInterval(const std::uint8_t* headPtr,
                                                               const std::uint8_t* varEndPtr) const
{
    // read header
    IntervalHeader header;
    std::memcpy(&header, headPtr, sizeof(header));
    headPtr += IntervalHeader::SIZE;

    // read type and ID
    auto type = header.getType();
    auto id = header.getId();

    // make sure a factory exists for this type
    if (_factories[type] == nullptr) {
        throw UnknownIntervalType {type};
    }

    // create associated interval
    auto interval = _factories[type]->create(header.begin, header.end, id);

    // deserialize values
    interval->deserializeValues(headPtr, varEndPtr);

    return std::move(interval);
}
