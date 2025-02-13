////////////////////////////////////////////////////////////////////////////////
/// @brief list of collection keys dumps present in database
///
/// @file
///
/// DISCLAIMER
///
/// Copyright 2014 ArangoDB GmbH, Cologne, Germany
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
/// @author Jan Steemann
/// @author Copyright 2014, ArangoDB GmbH, Cologne, Germany
/// @author Copyright 2012-2013, triAGENS GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

#ifndef ARANGODB_ARANGO_COLLECTION_KEYS_REPOSITORY_H
#define ARANGODB_ARANGO_COLLECTION_KEYS_REPOSITORY_H 1

#include "Basics/Common.h"
#include "Basics/Mutex.h"
#include "Utils/CollectionKeys.h"
#include "VocBase/voc-types.h"

struct TRI_json_t;

namespace triagens {
  namespace arango {
    
    class CollectionKeys;

// -----------------------------------------------------------------------------
// --SECTION--                                    class CollectionKeysRepository
// -----------------------------------------------------------------------------

    class CollectionKeysRepository {

// -----------------------------------------------------------------------------
// --SECTION--                                        constructors / destructors
// -----------------------------------------------------------------------------

      public:

////////////////////////////////////////////////////////////////////////////////
/// @brief create a collection keys repository
////////////////////////////////////////////////////////////////////////////////

        CollectionKeysRepository ();

////////////////////////////////////////////////////////////////////////////////
/// @brief destroy a collection keys repository
////////////////////////////////////////////////////////////////////////////////

        ~CollectionKeysRepository ();

// -----------------------------------------------------------------------------
// --SECTION--                                                    public methods
// -----------------------------------------------------------------------------

      public:

////////////////////////////////////////////////////////////////////////////////
/// @brief stores collection keys in the repository
////////////////////////////////////////////////////////////////////////////////

        void store (CollectionKeys*); 

////////////////////////////////////////////////////////////////////////////////
/// @brief remove a keys by id
////////////////////////////////////////////////////////////////////////////////

        bool remove (CollectionKeysId);

////////////////////////////////////////////////////////////////////////////////
/// @brief find an existing collection keys list by id
/// if found, the keys will be returned with the usage flag set to true. 
/// it must be returned later using release() 
////////////////////////////////////////////////////////////////////////////////

        CollectionKeys* find (CollectionKeysId);

////////////////////////////////////////////////////////////////////////////////
/// @brief return a cursor
////////////////////////////////////////////////////////////////////////////////

        void release (CollectionKeys*);

////////////////////////////////////////////////////////////////////////////////
/// @brief whether or not the repository contains used keys
////////////////////////////////////////////////////////////////////////////////

        bool containsUsed ();

////////////////////////////////////////////////////////////////////////////////
/// @brief run a garbage collection on the data
////////////////////////////////////////////////////////////////////////////////

        bool garbageCollect (bool);

// -----------------------------------------------------------------------------
// --SECTION--                                                 private variables
// -----------------------------------------------------------------------------

      private:

////////////////////////////////////////////////////////////////////////////////
/// @brief mutex for the repository
////////////////////////////////////////////////////////////////////////////////

        triagens::basics::Mutex _lock;

////////////////////////////////////////////////////////////////////////////////
/// @brief list of current keys
////////////////////////////////////////////////////////////////////////////////

        std::unordered_map<CollectionKeysId, CollectionKeys*> _keys;

////////////////////////////////////////////////////////////////////////////////
/// @brief maximum number of keys to garbage-collect in one go
////////////////////////////////////////////////////////////////////////////////

        static size_t const MaxCollectCount;
    };

  }
}

#endif

// -----------------------------------------------------------------------------
// --SECTION--                                                       END-OF-FILE
// -----------------------------------------------------------------------------

// Local Variables:
// mode: outline-minor
// outline-regexp: "/// @brief\\|/// {@inheritDoc}\\|/// @page\\|// --SECTION--\\|/// @\\}"
// End:
