module.define("org/arangodb/arango-query-cursor", function(exports, module) {
/*jshint strict: false */
/*global more:true */

////////////////////////////////////////////////////////////////////////////////
/// @brief ArangoQueryCursor
///
/// @file
///
/// DISCLAIMER
///
/// Copyright 2013 triagens GmbH, Cologne, Germany
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
/// @author Achim Brandt
/// @author Dr. Frank Celler
/// @author Copyright 2012-2013, triAGENS GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

var internal = require("internal");
var arangosh = require("org/arangodb/arangosh");

// -----------------------------------------------------------------------------
// --SECTION--                                                 ArangoQueryCursor
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// --SECTION--                                      constructors and destructors
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief constructor
////////////////////////////////////////////////////////////////////////////////

function ArangoQueryCursor (database, data) {
  this._database = database;
  this._dbName = database._name();
  this.data = data;
  this._hasNext = false;
  this._hasMore = false;
  this._pos = 0;
  this._count = 0;
  this._total = 0;

  if (data.result !== undefined) {
    this._count = data.result.length;

    if (this._pos < this._count) {
      this._hasNext = true;
    }

    if (data.hasMore !== undefined && data.hasMore) {
      this._hasMore = true;
    }
  }
}

exports.ArangoQueryCursor = ArangoQueryCursor;

// -----------------------------------------------------------------------------
// --SECTION--                                                 private functions
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief return a string representation of the cursor
////////////////////////////////////////////////////////////////////////////////

ArangoQueryCursor.prototype.toString = function () {
  var isCaptureModeActive = internal.isCaptureMode();
  var rows = [ ], i = 0;
  while (++i <= 10 && this.hasNext()) {
    rows.push(this.next());
  }

  var result = "[object ArangoQueryCursor";

  if (this.data.id) {
    result += " " + this.data.id;
  }

  if (this._count !== null && this._count !== undefined) {
    result += ", count: " + this._count;
  }

  result += ", hasMore: " + (this.hasNext() ? "true" : "false");

  result += "]";

  if (!isCaptureModeActive) {
    internal.print(result);
    result = "";
  }
  if (rows.length > 0) {
    if (!isCaptureModeActive) {
      var old = internal.startCaptureMode();
      internal.print(rows);
      result += "\n\n" + internal.stopCaptureMode(old);
    }
    else {
      internal.print(rows);
    }

    if (this.hasNext()) {
      result += "\ntype 'more' to show more documents\n";
      more = this; // assign cursor to global variable more!
    }
  }

  if (!isCaptureModeActive) {
    internal.print(result);
    result = "";
  }
  return result;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief return all remaining result documents from the cursor
///
/// If no more results are available locally but more results are available on
/// the server, this function will make one or multiple roundtrips to the
/// server. Calling this function will also fully exhaust the cursor.
////////////////////////////////////////////////////////////////////////////////

ArangoQueryCursor.prototype.toArray = function () {
  var result = [];

  while (this.hasNext()) {
    result.push(this.next());
  }

  return result;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief print the help for the cursor
////////////////////////////////////////////////////////////////////////////////

var helpArangoQueryCursor = arangosh.createHelpHeadline("ArangoQueryCursor help") +
  'ArangoQueryCursor constructor:                                      ' + "\n" +
  ' > cursor = stmt.execute()                                          ' + "\n" +
  'Functions:                                                          ' + "\n" +
  '  hasNext()                             returns true if there are   ' + "\n" +
  '                                        more results to fetch       ' + "\n" +
  '  next()                                returns the next document   ' + "\n" +
  '  toArray()                             returns all data from the cursor' + "\n" +
  '  _help()                               this help                   ' + "\n" +
  'Attributes:                                                         ' + "\n" +
  '  _database                             database object             ' + "\n" +
  'Example:                                                            ' + "\n" +
  ' > stmt = db._createStatement({ "query": "FOR c IN coll RETURN c" })' + "\n" +
  ' > cursor = stmt.execute()                                          ' + "\n" +
  ' > documents = cursor.toArray()                                     ' + "\n" +
  ' > cursor = stmt.execute()                                          ' + "\n" +
  ' > while (cursor.hasNext()) { print(cursor.next())  }               ';

ArangoQueryCursor.prototype._help = function () {
  internal.print(helpArangoQueryCursor);
};

// -----------------------------------------------------------------------------
// --SECTION--                                                  public functions
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief return whether there are more results available in the cursor
////////////////////////////////////////////////////////////////////////////////

ArangoQueryCursor.prototype.hasNext = function () {
  return this._hasNext;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief return the next result document from the cursor
///
/// If no more results are available locally but more results are available on
/// the server, this function will make a roundtrip to the server
////////////////////////////////////////////////////////////////////////////////

ArangoQueryCursor.prototype.next = function () {
  if (! this._hasNext) {
    throw "No more results";
  }

  var result = this.data.result[this._pos];
  this._pos++;

  // reached last result
  if (this._pos === this._count) {
    this._hasNext = false;
    this._pos = 0;

    if (this._hasMore && this.data.id) {
      this._hasMore = false;

      // load more results
      var requestResult = this._database._connection.PUT(this._baseurl(), "");

      arangosh.checkRequestResult(requestResult);

      this.data = requestResult;
      this._count = requestResult.result.length;

      if (this._pos < this._count) {
        this._hasNext = true;
      }

      if (requestResult.hasMore !== undefined && requestResult.hasMore) {
        this._hasMore = true;
      }
    }
  }

  return result;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief explicitly dispose the cursor
///
/// Calling this function will mark the cursor as deleted on the server. It will
/// therefore make a roundtrip to the server. Using a cursor after it has been
/// disposed is considered a user error
////////////////////////////////////////////////////////////////////////////////

ArangoQueryCursor.prototype.dispose = function () {
  if (! this.data.id) {
    // client side only cursor
    return;
  }

  var requestResult = this._database._connection.DELETE(this._baseurl(), "");

  arangosh.checkRequestResult(requestResult);

  this.data.id = undefined;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief return the total number of documents in the cursor
///
/// The number will remain the same regardless how much result documents have
/// already been fetched from the cursor.
///
/// This function will return the number only if the cursor was constructed
/// with the "doCount" attribute. Otherwise it will return undefined.
////////////////////////////////////////////////////////////////////////////////

ArangoQueryCursor.prototype.count = function () {
  return this.data.count;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief return extra data stored for the cursor (if any)
////////////////////////////////////////////////////////////////////////////////

ArangoQueryCursor.prototype.getExtra = function () {
  return this.data.extra || { };
};

////////////////////////////////////////////////////////////////////////////////
/// @brief return baseurl for query cursor
////////////////////////////////////////////////////////////////////////////////

ArangoQueryCursor.prototype._baseurl = function () {
  return "/_db/" + encodeURIComponent(this._dbName) +
         "/_api/cursor/" + encodeURIComponent(this.data.id);
};

// -----------------------------------------------------------------------------
// --SECTION--                                                       END-OF-FILE
// -----------------------------------------------------------------------------

// Local Variables:
// mode: outline-minor
// outline-regexp: "^\\(/// @brief\\|/// @addtogroup\\|// --SECTION--\\|/// @}\\|/\\*jslint\\)"
// End:
});
