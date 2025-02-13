/*jshint strict: false */

////////////////////////////////////////////////////////////////////////////////
/// @brief ArangoDatabase
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
// --SECTION--                                                    ArangoDatabase
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// --SECTION--                                      constructors and destructors
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief constructor
////////////////////////////////////////////////////////////////////////////////

var ArangoCollection;

function ArangoDatabase (connection) {
  this._connection = connection;
  this._collectionConstructor = ArangoCollection;
  this._properties = null;

  this._registerCollection = function (name, obj) {
    // store the collection in our own list
    this[name] = obj;
  };
}

exports.ArangoDatabase = ArangoDatabase;

// load after exporting ArangoDatabase
ArangoCollection = require("org/arangodb/arango-collection").ArangoCollection;
var ArangoError = require("org/arangodb").ArangoError;
var ArangoStatement = require("org/arangodb/arango-statement").ArangoStatement;

// -----------------------------------------------------------------------------
// --SECTION--                                                 private functions
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief index id regex
////////////////////////////////////////////////////////////////////////////////

ArangoDatabase.indexRegex = /^([a-zA-Z0-9\-_]+)\/([0-9]+)$/;

////////////////////////////////////////////////////////////////////////////////
/// @brief key regex
////////////////////////////////////////////////////////////////////////////////

ArangoDatabase.keyRegex = /^([a-zA-Z0-9_:\-@\.\(\)\+,=;\$!\*'%])+$/;

////////////////////////////////////////////////////////////////////////////////
/// @brief append the waitForSync parameter to a URL
////////////////////////////////////////////////////////////////////////////////

ArangoDatabase.prototype._appendSyncParameter = function (url, waitForSync) {
  if (waitForSync) {
    if (url.indexOf('?') === -1) {
      url += '?';
    }
    else {
      url += '&';
    }
    url += 'waitForSync=true';
  }
  return url;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief return the base url for collection usage
////////////////////////////////////////////////////////////////////////////////

ArangoDatabase.prototype._collectionurl = function (id) {
  if (id === undefined) {
    return "/_api/collection";
  }

  return "/_api/collection/" + encodeURIComponent(id);
};

////////////////////////////////////////////////////////////////////////////////
/// @brief return the base url for document usage
////////////////////////////////////////////////////////////////////////////////

ArangoDatabase.prototype._documenturl = function (id, expectedName) {
  var s = id.split("/");

  if (s.length !== 2) {
    throw new ArangoError({
      error: true,
      code: internal.errors.ERROR_HTTP_BAD_PARAMETER.code,
      errorNum: internal.errors.ERROR_ARANGO_DOCUMENT_HANDLE_BAD.code,
      errorMessage: internal.errors.ERROR_ARANGO_DOCUMENT_HANDLE_BAD.message
    });
  }
  else if (expectedName !== undefined && expectedName !== "" && s[0] !== expectedName) {
    throw new ArangoError({
      error: true,
      code: internal.errors.ERROR_HTTP_BAD_PARAMETER.code,
      errorNum: internal.errors.ERROR_ARANGO_CROSS_COLLECTION_REQUEST.code,
      errorMessage: internal.errors.ERROR_ARANGO_CROSS_COLLECTION_REQUEST.message
    });
  }

  if (ArangoDatabase.keyRegex.exec(s[1]) === null) {
    throw new ArangoError({
      error: true,
      code: internal.errors.ERROR_HTTP_BAD_PARAMETER.code,
      errorNum: internal.errors.ERROR_ARANGO_DOCUMENT_HANDLE_BAD.code,
      errorMessage: internal.errors.ERROR_ARANGO_DOCUMENT_HANDLE_BAD.message
    });
  }

  return "/_api/document/" + encodeURIComponent(s[0]) + "/" + encodeURIComponent(s[1]);
};

////////////////////////////////////////////////////////////////////////////////
/// @brief return the base url for index usage
////////////////////////////////////////////////////////////////////////////////

ArangoDatabase.prototype._indexurl = function (id, expectedName) {
  if (typeof id === "string") {
    var pa = ArangoDatabase.indexRegex.exec(id);

    if (pa === null && expectedName !== undefined) {
      id = expectedName + "/" + id;
    }
  }
  else if (typeof id === "number" && expectedName !== undefined) {
    // stringify a numeric id
    id = expectedName + "/" + id;
  }

  var s = id.split("/");

  if (s.length !== 2) {
    // invalid index handle
    throw new ArangoError({
      error: true,
      code: internal.errors.ERROR_HTTP_BAD_PARAMETER.code,
      errorNum: internal.errors.ERROR_ARANGO_INDEX_HANDLE_BAD.code,
      errorMessage: internal.errors.ERROR_ARANGO_INDEX_HANDLE_BAD.message
    });
  }
  else if (expectedName !== undefined && expectedName !== "" && s[0] !== expectedName) {
    // index handle does not match collection name
    throw new ArangoError({
      error: true,
      code: internal.errors.ERROR_HTTP_BAD_PARAMETER.code,
      errorNum: internal.errors.ERROR_ARANGO_CROSS_COLLECTION_REQUEST.code,
      errorMessage: internal.errors.ERROR_ARANGO_CROSS_COLLECTION_REQUEST.message
    });
  }

  return "/_api/index/" + encodeURIComponent(s[0]) + "/" + encodeURIComponent(s[1]);
};

////////////////////////////////////////////////////////////////////////////////
/// @brief prints the help for ArangoDatabase
////////////////////////////////////////////////////////////////////////////////

var helpArangoDatabase = arangosh.createHelpHeadline("ArangoDatabase (db) help") +
  'Administration Functions:                                                 ' + "\n" +
  '  _help()                               this help                         ' + "\n" +
  '  _flushCache()                         flush and refill collection cache ' + "\n" +
  '                                                                          ' + "\n" +
  'Collection Functions:                                                     ' + "\n" +
  '  _collections()                        list all collections              ' + "\n" +
  '  _collection(<name>)                   get collection by identifier/name ' + "\n" +
  '  _create(<name>, <properties>)         creates a new collection          ' + "\n" +
  '  _createEdgeCollection(<name>)         creates a new edge collection     ' + "\n" +
  '  _drop(<name>)                         delete a collection               ' + "\n" +
  '                                                                          ' + "\n" +
  'Document Functions:                                                       ' + "\n" +
  '  _document(<id>)                       get document by handle (_id)      ' + "\n" +
  '  _replace(<id>, <data>, <overwrite>)   overwrite document                ' + "\n" +
  '  _update(<id>, <data>, <overwrite>,    partially update document         ' + "\n" +
  '          <keepNull>)                                                     ' + "\n" +
  '  _remove(<id>)                         delete document                   ' + "\n" +
  '  _exists(<id>)                         checks whether a document exists  ' + "\n" +
  '  _truncate()                           delete all documents              ' + "\n" +
  '                                                                          ' + "\n" +
  'Database Management Functions:                                            ' + "\n" +
  '  _createDatabase(<name>)               creates a new database            ' + "\n" +
  '  _dropDatabase(<name>)                 drops an existing database        ' + "\n" +
  '  _useDatabase(<name>)                  switches into an existing database' + "\n" +
  '  _drop(<name>)                         delete a collection               ' + "\n" +
  '  _name()                               name of the current database      ' + "\n" +
  '                                                                          ' + "\n" +
  'Query / Transaction Functions:                                            ' + "\n" +
  '  _executeTransaction(<transaction>)    execute transaction               ' + "\n" +
  '  _query(<query>)                       execute AQL query                 ' + "\n" +
  '  _createStatement(<data>)              create and return AQL query       ';

ArangoDatabase.prototype._help = function () {
  internal.print(helpArangoDatabase);
};

////////////////////////////////////////////////////////////////////////////////
/// @brief return a string representation of the database object
////////////////////////////////////////////////////////////////////////////////

ArangoDatabase.prototype.toString = function () {
  return "[object ArangoDatabase \"" + this._name() + "\"]";
};

// -----------------------------------------------------------------------------
// --SECTION--                                              collection functions
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief return all collections from the database
////////////////////////////////////////////////////////////////////////////////

ArangoDatabase.prototype._collections = function () {
  var requestResult = this._connection.GET(this._collectionurl());

  arangosh.checkRequestResult(requestResult);

  if (requestResult.collections !== undefined) {
    var collections = requestResult.collections;
    var result = [];
    var i;

    // add all collentions to object
    for (i = 0;  i < collections.length;  ++i) {
      var collection = new this._collectionConstructor(this, collections[i]);
      this._registerCollection(collection._name, collection);
      result.push(collection);
    }

    return result;
  }

  return undefined;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief return a single collection, identified by its id or name
////////////////////////////////////////////////////////////////////////////////

ArangoDatabase.prototype._collection = function (id) {
  var url;

  if (typeof id === "number") {
    url = this._collectionurl(id) + "?useId=true";
  }
  else {
    url = this._collectionurl(id);
  }

  var requestResult = this._connection.GET(url);

  // return null in case of not found
  if (requestResult !== null
      && requestResult.error === true
      && requestResult.errorNum === internal.errors.ERROR_ARANGO_COLLECTION_NOT_FOUND.code) {
    return null;
  }

  // check all other errors and throw them
  arangosh.checkRequestResult(requestResult);

  var name = requestResult.name;

  if (name !== undefined) {
    this._registerCollection(name, new this._collectionConstructor(this, requestResult));
    return this[name];
  }

  return null;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief creates a new collection
////////////////////////////////////////////////////////////////////////////////

ArangoDatabase.prototype._create = function (name, properties, type) {
  var body = {
    "name" : name,
    "type" : ArangoCollection.TYPE_DOCUMENT
  };

  if (properties !== undefined) {
    [ "waitForSync", "journalSize", "isSystem", "isVolatile",
      "doCompact", "keyOptions", "shardKeys", "numberOfShards",
      "distributeShardsLike", "indexBuckets" ].forEach(function(p) {
      if (properties.hasOwnProperty(p)) {
        body[p] = properties[p];
      }
    });
  }

  if (type !== undefined) {
    body.type = type;
  }

  var requestResult = this._connection.POST(this._collectionurl(),
    JSON.stringify(body));

  arangosh.checkRequestResult(requestResult);

  var nname = requestResult.name;

  if (nname !== undefined) {
    this._registerCollection(nname, new this._collectionConstructor(this, requestResult));
    return this[nname];
  }

  return undefined;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief creates a new document collection
////////////////////////////////////////////////////////////////////////////////

ArangoDatabase.prototype._createDocumentCollection = function (name, properties) {
  return this._create(name, properties, ArangoCollection.TYPE_DOCUMENT);
};

////////////////////////////////////////////////////////////////////////////////
/// @brief creates a new edges collection
////////////////////////////////////////////////////////////////////////////////

ArangoDatabase.prototype._createEdgeCollection = function (name, properties) {
  return this._create(name, properties, ArangoCollection.TYPE_EDGE);
};

////////////////////////////////////////////////////////////////////////////////
/// @brief truncates a collection
////////////////////////////////////////////////////////////////////////////////

ArangoDatabase.prototype._truncate = function (id) {
  var name;

  if (typeof id !== "string") {
    id = id._id;
  }

  for (name in this) {
    if (this.hasOwnProperty(name)) {
      var collection = this[name];

      if (collection instanceof this._collectionConstructor) {
        if (collection._id === id || collection._name === id) {
          return collection.truncate();
        }
      }
    }
  }

  return undefined;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief drops a collection
////////////////////////////////////////////////////////////////////////////////

ArangoDatabase.prototype._drop = function (id) {
  var name;

  for (name in this) {
    if (this.hasOwnProperty(name)) {
      var collection = this[name];

      if (collection instanceof this._collectionConstructor) {
        if (collection._id === id || collection._name === id) {
          return collection.drop();
        }
      }
    }
  }

  var c = this._collection(id);
  if (c) {
    return c.drop();
  }
  return undefined;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief flush the local cache
/// this is called by connection.reconnect()
////////////////////////////////////////////////////////////////////////////////

ArangoDatabase.prototype._flushCache = function () {
  var name;

  for (name in this) {
    if (this.hasOwnProperty(name)) {
      var collection = this[name];

      if (collection instanceof this._collectionConstructor) {
        // reset the collection status
        collection._status = null;
        this[name] = undefined;
      }
    }
  }

  try {
    // repopulate cache
    this._collections();
  }
  catch (err) {
  }

  this._properties = null;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief query the database properties
////////////////////////////////////////////////////////////////////////////////

ArangoDatabase.prototype._queryProperties = function (force) {
  if (force || this._properties === null) {
    var url = "/_api/database/current";
    var requestResult = this._connection.GET(url);

    arangosh.checkRequestResult(requestResult);
    this._properties = requestResult.result;
  }

  return this._properties;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief return the database id
////////////////////////////////////////////////////////////////////////////////

ArangoDatabase.prototype._id = function () {
  return this._queryProperties().id;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief return whether or not the current database is the system database
////////////////////////////////////////////////////////////////////////////////

ArangoDatabase.prototype._isSystem = function () {
  return this._queryProperties().isSystem;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief get the name of the current database
////////////////////////////////////////////////////////////////////////////////

ArangoDatabase.prototype._name = function () {
  return this._queryProperties().name;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief get the path of the current database
////////////////////////////////////////////////////////////////////////////////

ArangoDatabase.prototype._path = function () {
  return this._queryProperties().path;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief returns one index
////////////////////////////////////////////////////////////////////////////////

ArangoDatabase.prototype._index = function (id) {
  if (id.hasOwnProperty("id")) {
    id = id.id;
  }

  var requestResult = this._connection.GET(this._indexurl(id));

  arangosh.checkRequestResult(requestResult);

  return requestResult;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief deletes one index
////////////////////////////////////////////////////////////////////////////////

ArangoDatabase.prototype._dropIndex = function (id) {
  if (id.hasOwnProperty("id")) {
    id = id.id;
  }

  var requestResult = this._connection.DELETE(this._indexurl(id));

  if (requestResult !== null
      && requestResult.error === true
      && requestResult.errorNum === internal.errors.ERROR_ARANGO_INDEX_NOT_FOUND.code) {
    return false;
  }

  arangosh.checkRequestResult(requestResult);

  return true;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief returns the database version
////////////////////////////////////////////////////////////////////////////////

ArangoDatabase.prototype._version = function () {
  var requestResult = this._connection.GET("/_api/version");

  arangosh.checkRequestResult(requestResult);

  return requestResult.version;
};

// -----------------------------------------------------------------------------
// --SECTION--                                                document functions
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief return a single document from the collection, identified by its id
////////////////////////////////////////////////////////////////////////////////

ArangoDatabase.prototype._document = function (id) {
  var rev = null;
  var requestResult;

  if (id.hasOwnProperty("_id")) {
    if (id.hasOwnProperty("_rev")) {
      rev = id._rev;
    }

    id = id._id;
  }

  if (rev === null) {
    requestResult = this._connection.GET(this._documenturl(id));
  }
  else {
    requestResult = this._connection.GET(this._documenturl(id),
      {'if-match' : JSON.stringify(rev) });
  }

  if (requestResult !== null
      && requestResult.error === true
      && requestResult.errorNum === internal.errors.ERROR_ARANGO_COLLECTION_NOT_FOUND.code) {
    throw new ArangoError(requestResult);
  }

  arangosh.checkRequestResult(requestResult);

  return requestResult;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief checks whether a document exists, identified by its id
////////////////////////////////////////////////////////////////////////////////

ArangoDatabase.prototype._exists = function (id) {
  var rev = null;
  var requestResult;

  if (id.hasOwnProperty("_id")) {
    if (id.hasOwnProperty("_rev")) {
      rev = id._rev;
    }

    id = id._id;
  }

  if (rev === null) {
    requestResult = this._connection.HEAD(this._documenturl(id));
  }
  else {
    requestResult = this._connection.HEAD(this._documenturl(id),
      {'if-match' : JSON.stringify(rev) });
  }

  if (requestResult !== null &&
      requestResult.error === true &&
      (requestResult.errorNum === internal.errors.ERROR_ARANGO_COLLECTION_NOT_FOUND.code ||
       requestResult.errorNum === internal.errors.ERROR_HTTP_NOT_FOUND.code ||
       requestResult.errorNum === internal.errors.ERROR_HTTP_PRECONDITION_FAILED.code)) {
    return false;
  }

  arangosh.checkRequestResult(requestResult);

  return true;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief delete a document in the collection, identified by its id
////////////////////////////////////////////////////////////////////////////////

ArangoDatabase.prototype._remove = function (id, overwrite, waitForSync) {
  var rev = null;
  var requestResult;

  if (id.hasOwnProperty("_id")) {
    if (id.hasOwnProperty("_rev")) {
      rev = id._rev;
    }

    id = id._id;
  }

  var params = "";

  if (typeof overwrite === "object") {
    if (typeof waitForSync !== "undefined") {
      throw "too many arguments";
    }
    // we assume the caller uses new signature (id, data, options)
    var options = overwrite;
    if (options.hasOwnProperty("overwrite") && options.overwrite) {
      params += "?policy=last";
    }
    if (options.hasOwnProperty("waitForSync") ) {
      waitForSync = options.waitForSync;
    }
  } else {
    if (overwrite) {
      params += "?policy=last";
    }
  }

  var url = this._documenturl(id) + params;
  url = this._appendSyncParameter(url, waitForSync);

  if (rev === null) {
    requestResult = this._connection.DELETE(url);
  }
  else {
    requestResult = this._connection.DELETE(url,
      {'if-match' : JSON.stringify(rev) });
  }

  if (requestResult !== null && requestResult.error === true) {
    if (overwrite) {
      if (requestResult.errorNum === internal.errors.ERROR_ARANGO_DOCUMENT_NOT_FOUND.code) {
        return false;
      }
    }

    throw new ArangoError(requestResult);
  }

  arangosh.checkRequestResult(requestResult);

  return true;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief replace a document in the collection, identified by its id
////////////////////////////////////////////////////////////////////////////////

ArangoDatabase.prototype._replace = function (id, data, overwrite, waitForSync) {
  var rev = null;
  var requestResult;

  if (id.hasOwnProperty("_id")) {
    if (id.hasOwnProperty("_rev")) {
      rev = id._rev;
    }

    id = id._id;
  }

  var params = "";

  if (typeof overwrite === "object") {
    if (typeof waitForSync !== "undefined") {
      throw "too many arguments";
    }
    // we assume the caller uses new signature (id, data, options)
    var options = overwrite;
    if (options.hasOwnProperty("overwrite") && options.overwrite) {
      params += "?policy=last";
    }
    if (options.hasOwnProperty("waitForSync") ) {
     waitForSync = options.waitForSync;
    }
  } else {
    if (overwrite) {
      params += "?policy=last";
    }
  }
  var url = this._documenturl(id) + params;
  url = this._appendSyncParameter(url, waitForSync);

  if (rev === null) {
    requestResult = this._connection.PUT(url, JSON.stringify(data));
  }
  else {
    requestResult = this._connection.PUT(url, JSON.stringify(data),
      {'if-match' : JSON.stringify(rev) });
  }

  if (requestResult !== null && requestResult.error === true) {
    throw new ArangoError(requestResult);
  }

  arangosh.checkRequestResult(requestResult);

  return requestResult;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief update a document in the collection, identified by its id
////////////////////////////////////////////////////////////////////////////////

ArangoDatabase.prototype._update = function (id, data, overwrite, keepNull, waitForSync) {
  var rev = null;
  var requestResult;

  if (id.hasOwnProperty("_id")) {
    if (id.hasOwnProperty("_rev")) {
      rev = id._rev;
    }

    id = id._id;
  }

  var params = "";
  if (typeof overwrite === "object") {
    if (typeof keepNull !== "undefined") {
      throw "too many arguments";
    }
    // we assume the caller uses new signature (id, data, options)
    var options = overwrite;
    if (! options.hasOwnProperty("keepNull")) {
      options.keepNull = true;
    }
    params = "?keepNull=" + options.keepNull;
    if (! options.hasOwnProperty("mergeObjects")) {
      options.mergeObjects = true;
    }
    params += "&mergeObjects=" + options.mergeObjects;

    if (options.hasOwnProperty("overwrite") && options.overwrite) {
      params += "&policy=last";
    }
  } else {
    // set default value for keepNull
    var keepNullValue = ((typeof keepNull === "undefined") ? true : keepNull);
    params = "?keepNull=" + (keepNullValue ? "true" : "false");

    if (overwrite) {
      params += "&policy=last";
    }
  }
  var url = this._documenturl(id) + params;
  url = this._appendSyncParameter(url, waitForSync);

  if (rev === null) {
    requestResult = this._connection.PATCH(url, JSON.stringify(data));
  }
  else {
    requestResult = this._connection.PATCH(url, JSON.stringify(data),
      {'if-match' : JSON.stringify(rev) });
  }

  if (requestResult !== null && requestResult.error === true) {
    throw new ArangoError(requestResult);
  }

  arangosh.checkRequestResult(requestResult);

  return requestResult;
};

// -----------------------------------------------------------------------------
// --SECTION--                                                   query functions
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief factory method to create a new statement
////////////////////////////////////////////////////////////////////////////////

ArangoDatabase.prototype._createStatement = function (data) {
  return new ArangoStatement(this, data);
};

////////////////////////////////////////////////////////////////////////////////
/// @brief factory method to create and execute a new statement
////////////////////////////////////////////////////////////////////////////////

ArangoDatabase.prototype._query = function (query, bindVars, cursorOptions, options) {
  if (typeof query === "object" && query !== null && arguments.length === 1) {
    return new ArangoStatement(this, query).execute();
  }

  var data = {
    query: query,
    bindVars: bindVars || undefined,
    count: (cursorOptions && cursorOptions.count) || false,
    batchSize: (cursorOptions && cursorOptions.batchSize) || undefined,
    options: options || undefined,
    cache: (options && options.cache) || undefined
  };

  return new ArangoStatement(this, data).execute();
};

// -----------------------------------------------------------------------------
// --SECTION--                                                database functions
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief create a new database
////////////////////////////////////////////////////////////////////////////////

ArangoDatabase.prototype._createDatabase = function (name, options, users) {
  var data = {
    name: name,
    options: options || { },
    users: users || [ ]
  };

  var requestResult = this._connection.POST("/_api/database", JSON.stringify(data));

  if (requestResult !== null && requestResult.error === true) {
    throw new ArangoError(requestResult);
  }

  arangosh.checkRequestResult(requestResult);

  return requestResult.result;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief drop an existing database
////////////////////////////////////////////////////////////////////////////////

ArangoDatabase.prototype._dropDatabase = function (name) {
  var requestResult = this._connection.DELETE("/_api/database/" + encodeURIComponent(name));

  if (requestResult !== null && requestResult.error === true) {
    throw new ArangoError(requestResult);
  }

  arangosh.checkRequestResult(requestResult);

  return requestResult.result;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief list all existing databases
////////////////////////////////////////////////////////////////////////////////

ArangoDatabase.prototype._listDatabases = function () {
  var requestResult = this._connection.GET("/_api/database");

  if (requestResult !== null && requestResult.error === true) {
    throw new ArangoError(requestResult);
  }

  arangosh.checkRequestResult(requestResult);

  return requestResult.result;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief uses a database
////////////////////////////////////////////////////////////////////////////////

ArangoDatabase.prototype._useDatabase = function (name) {
  if (internal.printBrowser) {
    throw new ArangoError({
      error: true,
      code: internal.errors.ERROR_NOT_IMPLEMENTED.code,
      errorNum: internal.errors.ERROR_NOT_IMPLEMENTED.code,
      errorMessage: "_useDatabase() is not supported in the web interface"
    });
  }

  var old = this._connection.getDatabaseName();

  // no change
  if (name === old) {
    return true;
  }

  this._connection.setDatabaseName(name);

  try {
    // re-query properties
    this._queryProperties(true);
    this._flushCache();
  }
  catch (err) {
    this._connection.setDatabaseName(old);

    if (err.hasOwnProperty("errorNum")) {
      throw err;
    }

    throw new ArangoError({
      error: true,
      code: internal.errors.ERROR_BAD_PARAMETER.code,
      errorNum: internal.errors.ERROR_BAD_PARAMETER.code,
      errorMessage: "cannot use database '" + name + "'"
    });
  }

  return true;
};

// -----------------------------------------------------------------------------
// --SECTION--                                                         endpoints
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief lists all endpoints
////////////////////////////////////////////////////////////////////////////////

ArangoDatabase.prototype._listEndpoints = function () {
  var requestResult = this._connection.GET("/_api/endpoint");

  if (requestResult !== null && requestResult.error === true) {
    throw new ArangoError(requestResult);
  }

  arangosh.checkRequestResult(requestResult);

  return requestResult;
};

// -----------------------------------------------------------------------------
// --SECTION--                                                      transactions
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief execute a transaction
////////////////////////////////////////////////////////////////////////////////

ArangoDatabase.prototype._executeTransaction = function (data) {
  if (! data || typeof(data) !== 'object') {
    throw new ArangoError({
      error: true,
      code: internal.errors.ERROR_HTTP_BAD_PARAMETER.code,
      errorNum: internal.errors.ERROR_BAD_PARAMETER.code,
      errorMessage: "usage: _executeTransaction(<object>)"
    });
  }

  if (! data.collections || typeof(data.collections) !== 'object') {
    throw new ArangoError({
      error: true,
      code: internal.errors.ERROR_HTTP_BAD_PARAMETER.code,
      errorNum: internal.errors.ERROR_BAD_PARAMETER.code,
      errorMessage: "missing/invalid collections definition for transaction"
    });
  }

  if (! data.action ||
      (typeof(data.action) !== 'string' && typeof(data.action) !== 'function')) {
    throw new ArangoError({
      error: true,
      code: internal.errors.ERROR_HTTP_BAD_PARAMETER.code,
      errorNum: internal.errors.ERROR_BAD_PARAMETER.code,
      errorMessage: "missing/invalid action definition for transaction"
    });
  }

  if (typeof(data.action) === 'function') {
    data.action = String(data.action);
  }

  var requestResult = this._connection.POST("/_api/transaction",
    JSON.stringify(data));

  if (requestResult !== null && requestResult.error === true) {
    throw new ArangoError(requestResult);
  }

  arangosh.checkRequestResult(requestResult);

  return requestResult.result;
};

// -----------------------------------------------------------------------------
// --SECTION--                                                       END-OF-FILE
// -----------------------------------------------------------------------------

// Local Variables:
// mode: outline-minor
// outline-regexp: "/// @brief\\|/// @addtogroup\\|// --SECTION--\\|/// @}\\|/\\*jslint"
// End:
