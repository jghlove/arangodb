////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2014-2017 ArangoDB GmbH, Cologne, Germany
/// Copyright 2004-2014 triAGENS GmbH, Cologne, Germany
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
/// @author Daniel H. Larkin
/// @author Jan Christoph Uhde
////////////////////////////////////////////////////////////////////////////////

#ifndef ARANGO_ROCKSDB_ROCKSDB_TYPES_H
#define ARANGO_ROCKSDB_ROCKSDB_TYPES_H 1

#include <rocksdb/slice.h>
#include <cstdint>

namespace arangodb {


enum class RocksDBEntryType : char {
  Database = '0',
  Collection = '1',
  Index = '2',
  Document = '3',
  IndexValue = '4',
  UniqueIndexValue = '5',
  View = '6',
  CrossReference = '9'
};

rocksdb::Slice const& rocksDBSlice(RocksDBEntryType const& type);
}

#endif