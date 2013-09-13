////////////////////////////////////////////////////////////////////////////////
/// @brief test the server-side database interface
///
/// @file
///
/// DISCLAIMER
///
/// Copyright 2010-2012 triagens GmbH, Cologne, Germany
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
/// Copyright holder is triAGENS GmbH, Cologne, Germany
///
/// @author Jan Steemann
/// @author Copyright 2012, triAGENS GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

var jsunity = require("jsunity");
var internal = require("internal");
var arangodb = require("org/arangodb");
var ERRORS = arangodb.errors;

// -----------------------------------------------------------------------------
// --SECTION--                                                  database methods
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief test suite: dropping databases while holding references
////////////////////////////////////////////////////////////////////////////////

function DatabaseSuite () {
  return {

    setUp : function () {
      internal.db._useDatabase("_system");
    },

////////////////////////////////////////////////////////////////////////////////
/// @brief test references helt on dropped database collections
////////////////////////////////////////////////////////////////////////////////

    testDropDatabaseCollectionReferences : function () {
      assertEqual("_system", internal.db._name());

      try {
        internal.db._dropDatabase("UnitTestsDatabase0");
      }
      catch (err) {
      }

      assertTrue(internal.db._createDatabase("UnitTestsDatabase0"));

      internal.db._useDatabase("UnitTestsDatabase0");
      assertEqual("UnitTestsDatabase0", internal.db._name());

      // insert 1000 docs and hold a reference on the data
      var c = internal.db._create("test");
      for (var i = 0; i < 1000; ++i) {
        c.save({ "_key": "test" + i, "value" : i });
      }
      assertEqual(1000, c.count());

      internal.db._useDatabase("_system");
      assertEqual(1000, c.count());

      // drop the database 
      internal.db._dropDatabase("UnitTestsDatabase0");
      // should be dropped
      internal.db._listDatabases().forEach(function (d) {
        if (d === "UnitTestsDatabase0") {
          fail();
        }
      });

      // collection should still be there
      assertEqual(1000, c.count());
      assertEqual("test", c.name());

      internal.wait(5);
      // still...
      assertEqual(1000, c.count());

      c = null;
    },

////////////////////////////////////////////////////////////////////////////////
/// @brief test references helt on documents of dropped databases
////////////////////////////////////////////////////////////////////////////////

    testDropDatabaseDocumentReferences : function () {
      assertEqual("_system", internal.db._name());

      try {
        internal.db._dropDatabase("UnitTestsDatabase0");
      }
      catch (err) {
      }

      assertTrue(internal.db._createDatabase("UnitTestsDatabase0"));

      internal.db._useDatabase("UnitTestsDatabase0");
      assertEqual("UnitTestsDatabase0", internal.db._name());

      // insert docs and hold a reference on the data
      var c = internal.db._create("test");
      for (var i = 0; i < 10; ++i) {
        c.save({ "_key": "test" + i, "value" : i });
      }

      var d0 = c.document("test0");
      var d4 = c.document("test4");
      var d9 = c.document("test9");

      c = null;

      internal.db._useDatabase("_system");

      // drop the database 
      internal.db._dropDatabase("UnitTestsDatabase0");
      // should be dropped
      internal.db._listDatabases().forEach(function (d) {
        if (d === "UnitTestsDatabase0") {
          fail();
        }
      });

      assertEqual(0, d0.value);
      assertEqual(4, d4.value);
      assertEqual(9, d9.value);

      internal.wait(5);
      
      assertEqual(0, d0.value);
      assertEqual(4, d4.value);
      assertEqual(9, d9.value);

      d0 = null;
      d4 = null;

      internal.wait(3);
      assertEqual(9, d9.value);

      d9 = null;
    }

  };
}

// -----------------------------------------------------------------------------
// --SECTION--                                                              main
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief executes the test suite
////////////////////////////////////////////////////////////////////////////////

jsunity.run(DatabaseSuite);

return jsunity.done();

// Local Variables:
// mode: outline-minor
// outline-regexp: "^\\(/// @brief\\|/// @addtogroup\\|// --SECTION--\\|/// @page\\|/// @}\\)"
// End:

