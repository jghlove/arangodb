////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2016 ArangoDB GmbH, Cologne, Germany
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// Copyright holder is ArangoDB GmbH, Cologne, Germany
///
/// @author Simon Grätzer
////////////////////////////////////////////////////////////////////////////////

#include "Pregel/AggregatorHandler.h"
#include "Basics/ReadLocker.h"
#include "Basics/WriteLocker.h"
#include "Pregel/Aggregator.h"
#include "Pregel/Algorithm.h"

using namespace arangodb;
using namespace arangodb::pregel;

AggregatorHandler::~AggregatorHandler() {
  WRITE_LOCKER(guard, _lock);
  for (auto const& it : _values) {
    delete it.second;
  }
  _values.clear();
}

IAggregator* AggregatorHandler::_get(AggregatorID const& name) {
  {
    READ_LOCKER(guard, _lock);
    auto it = _values.find(name);
    if (it != _values.end()) {
      return it->second;
    }
  }
  // aggregator doesn't exists, create it
  {
    WRITE_LOCKER(guard, _lock);
    std::unique_ptr<IAggregator> agg(_algorithm->aggregator(name));
    if (agg) {
      _values[name] = agg.get();
      return agg.release();
    }
  }
  return nullptr;
}

void AggregatorHandler::aggregate(AggregatorID const& name,
                                  const void* valuePtr) {
  IAggregator* agg = _get(name);
  if (agg) {
    agg->aggregate(valuePtr);
  }
}

const void* AggregatorHandler::getAggregatedValue(AggregatorID const& name) {
  IAggregator* agg = _get(name);
  return agg != nullptr ? agg->getValue() : nullptr;
}

void AggregatorHandler::resetValues(bool force) {
  for (auto& it : _values) {
    it.second->reset(force);
  }
}

void AggregatorHandler::aggregateValues(AggregatorHandler const& workerValues) {
  for (auto const& pair : workerValues._values) {
    AggregatorID const& name = pair.first;
    IAggregator* agg = _get(name);
    if (agg) {
      agg->aggregate(pair.second->getValue());
    }
  }
}

bool AggregatorHandler::parseValues(VPackSlice data) {
  VPackSlice values = data.get(Utils::aggregatorValuesKey);
  if (values.isObject() == false) {
    return false;
  }
  for (auto const& keyValue : VPackObjectIterator(values)) {
    AggregatorID name = keyValue.key.copyString();
    IAggregator* agg = _get(name);
    if (agg) {
      agg->parse(keyValue.value);
    }
  }
  return true;
}

bool AggregatorHandler::serializeValues(VPackBuilder& b,
                                        bool onlyConverging) const {
  READ_LOCKER(guard, _lock);
  bool hasValues = false;
  b.add(Utils::aggregatorValuesKey, VPackValue(VPackValueType::Object));
  for (auto const& pair : _values) {
    IAggregator* agg = pair.second;
    if (!onlyConverging || agg->isConverging()) {
      b.add(pair.first, agg->vpackValue());
      hasValues = true;
    }
  }
  b.close();
  return hasValues;
}

size_t AggregatorHandler::size() const {
  READ_LOCKER(guard, _lock);
  return _values.size();
}