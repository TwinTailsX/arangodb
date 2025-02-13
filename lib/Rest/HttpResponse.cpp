////////////////////////////////////////////////////////////////////////////////
/// @brief http response
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
/// @author Dr. Frank Celler
/// @author Achim Brandt
/// @author Copyright 2014, ArangoDB GmbH, Cologne, Germany
/// @author Copyright 2008-2013, triAGENS GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

#include "HttpResponse.h"
#include "Basics/logging.h"
#include "Basics/StringUtils.h"
#include "Basics/tri-strings.h"

using namespace triagens::basics;
using namespace triagens::rest;
using namespace std;

// -----------------------------------------------------------------------------
// --SECTION--                                             static public methods
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief batch error count header
////////////////////////////////////////////////////////////////////////////////

std::string const HttpResponse::BatchErrorHeader = "X-Arango-Errors";

////////////////////////////////////////////////////////////////////////////////
/// @brief http response string
////////////////////////////////////////////////////////////////////////////////

string HttpResponse::responseString (HttpResponseCode code) {
  switch (code) {
    //  Informational 1xx
    case CONTINUE:                        return "100 Continue";
    case SWITCHING_PROTOCOLS:             return "101 Switching Protocols";
    case PROCESSING:                      return "102 Processing";

    //  Success 2xx
    case OK:                              return "200 OK";
    case CREATED:                         return "201 Created";
    case ACCEPTED:                        return "202 Accepted";
    case PARTIAL:                         return "203 Non-Authoritative Information";
    case NO_CONTENT:                      return "204 No Content";
    case RESET_CONTENT:                   return "205 Reset Content";
    case PARTIAL_CONTENT:                 return "206 Partial Content";

    //  Redirection 3xx
    case MOVED_PERMANENTLY:               return "301 Moved Permanently";
    case FOUND:                           return "302 Found";
    case SEE_OTHER:                       return "303 See Other";
    case NOT_MODIFIED:                    return "304 Not Modified";
    case TEMPORARY_REDIRECT:              return "307 Temporary Redirect";
    case PERMANENT_REDIRECT:              return "308 Permanent Redirect";

    //  Error 4xx, 5xx
    case BAD:                             return "400 Bad Request";
    case UNAUTHORIZED:                    return "401 Unauthorized";
    case PAYMENT_REQUIRED:                return "402 Payment Required";
    case FORBIDDEN:                       return "403 Forbidden";
    case NOT_FOUND:                       return "404 Not Found";
    case METHOD_NOT_ALLOWED:              return "405 Method Not Allowed";
    case NOT_ACCEPTABLE:                  return "406 Not Acceptable";
    case REQUEST_TIMEOUT:                 return "408 Request Timeout";
    case CONFLICT:                        return "409 Conflict";
    case GONE:                            return "410 Gone";
    case LENGTH_REQUIRED:                 return "411 Length Required";
    case PRECONDITION_FAILED:             return "412 Precondition Failed";
    case REQUEST_ENTITY_TOO_LARGE:        return "413 Payload Too Large";
    case REQUEST_URI_TOO_LONG:            return "414 Request-URI Too Long";
    case UNSUPPORTED_MEDIA_TYPE:          return "415 Unsupported Media Type";
    case REQUESTED_RANGE_NOT_SATISFIABLE: return "416 Requested Range Not Satisfiable";
    case EXPECTATION_FAILED:              return "417 Expectation Failed";
    case I_AM_A_TEAPOT:                   return "418 I'm a teapot";
    case UNPROCESSABLE_ENTITY:            return "422 Unprocessable Entity";
    case LOCKED:                          return "423 Locked";
    case PRECONDITION_REQUIRED:           return "428 Precondition Required";
    case TOO_MANY_REQUESTS:               return "429 Too Many Requests";
    case REQUEST_HEADER_FIELDS_TOO_LARGE: return "431 Request Header Fields Too Large";

    case SERVER_ERROR:                    return "500 Internal Server Error";
    case NOT_IMPLEMENTED:                 return "501 Not Implemented";
    case BAD_GATEWAY:                     return "502 Bad Gateway";
    case SERVICE_UNAVAILABLE:             return "503 Service Unavailable";
    case HTTP_VERSION_NOT_SUPPORTED:      return "505 HTTP Version Not Supported";
    case BANDWIDTH_LIMIT_EXCEEDED:        return "509 Bandwidth Limit Exceeded";
    case NOT_EXTENDED:                    return "510 Not Extended";

    // default
    default: {
      // print generic group responses, based on error code group
      int group = ((int) code) / 100;
      switch (group) {
        case 1:
          return StringUtils::itoa((int) code) + " Informational";
        case 2:
          return StringUtils::itoa((int) code) + " Success";
        case 3:
          return StringUtils::itoa((int) code) + " Redirection";
        case 4:
          return StringUtils::itoa((int) code) + " Client error";
        case 5:
          return StringUtils::itoa((int) code) + " Server error";
        default:
          break;
      }
    }
  }
          
  return StringUtils::itoa((int) code) + " Unknown";
}

////////////////////////////////////////////////////////////////////////////////
/// @brief get http response code from string
////////////////////////////////////////////////////////////////////////////////

HttpResponse::HttpResponseCode HttpResponse::responseCode (const string& str) {
  int number = ::atoi(str.c_str());

  switch (number) {
    case 100: return CONTINUE;
    case 101: return SWITCHING_PROTOCOLS;
    case 102: return PROCESSING;

    case 200: return OK;
    case 201: return CREATED;
    case 202: return ACCEPTED;
    case 203: return PARTIAL;
    case 204: return NO_CONTENT;
    case 205: return RESET_CONTENT;
    case 206: return PARTIAL_CONTENT;

    case 301: return MOVED_PERMANENTLY;
    case 302: return FOUND;
    case 303: return SEE_OTHER;
    case 304: return NOT_MODIFIED;
    case 307: return TEMPORARY_REDIRECT;
    case 308: return PERMANENT_REDIRECT;

    case 400: return BAD;
    case 401: return UNAUTHORIZED;
    case 402: return PAYMENT_REQUIRED;
    case 403: return FORBIDDEN;
    case 404: return NOT_FOUND;
    case 405: return METHOD_NOT_ALLOWED;
    case 406: return NOT_ACCEPTABLE;
    case 408: return REQUEST_TIMEOUT;
    case 409: return CONFLICT;
    case 410: return GONE;
    case 411: return LENGTH_REQUIRED;
    case 412: return PRECONDITION_FAILED;
    case 413: return REQUEST_ENTITY_TOO_LARGE;
    case 414: return REQUEST_URI_TOO_LONG;
    case 415: return UNSUPPORTED_MEDIA_TYPE;
    case 416: return REQUESTED_RANGE_NOT_SATISFIABLE;
    case 417: return EXPECTATION_FAILED;
    case 418: return I_AM_A_TEAPOT;
    case 422: return UNPROCESSABLE_ENTITY;
    case 423: return LOCKED;
    case 428: return PRECONDITION_REQUIRED;
    case 429: return TOO_MANY_REQUESTS;
    case 431: return REQUEST_HEADER_FIELDS_TOO_LARGE;

    case 500: return SERVER_ERROR;
    case 501: return NOT_IMPLEMENTED;
    case 502: return BAD_GATEWAY;
    case 503: return SERVICE_UNAVAILABLE;
    case 505: return HTTP_VERSION_NOT_SUPPORTED;
    case 509: return BANDWIDTH_LIMIT_EXCEEDED;
    case 510: return NOT_EXTENDED;

    default:  return NOT_IMPLEMENTED;
  }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief get http response code from integer error code
////////////////////////////////////////////////////////////////////////////////

HttpResponse::HttpResponseCode HttpResponse::responseCode (int code) {
  TRI_ASSERT(code != TRI_ERROR_NO_ERROR);

  switch (code) {
    case TRI_ERROR_BAD_PARAMETER:
    case TRI_ERROR_ARANGO_DOCUMENT_KEY_BAD:
    case TRI_ERROR_ARANGO_DOCUMENT_KEY_UNEXPECTED:
    case TRI_ERROR_ARANGO_DOCUMENT_TYPE_INVALID:
    case TRI_ERROR_CLUSTER_MUST_NOT_CHANGE_SHARDING_ATTRIBUTES:
    case TRI_ERROR_CLUSTER_MUST_NOT_SPECIFY_KEY: 
    case TRI_ERROR_TYPE_ERROR: 
    case TRI_ERROR_QUERY_NUMBER_OUT_OF_RANGE:
    case TRI_ERROR_REPLICATION_INVALID_APPLIER_CONFIGURATION:
    case TRI_ERROR_REPLICATION_RUNNING:
      return BAD;
    
    case TRI_ERROR_ARANGO_READ_ONLY:
      return FORBIDDEN;
    
    case TRI_ERROR_ARANGO_COLLECTION_NOT_FOUND:
    case TRI_ERROR_ARANGO_DOCUMENT_NOT_FOUND:
    case TRI_ERROR_CURSOR_NOT_FOUND:
      return NOT_FOUND;

    case TRI_ERROR_REQUEST_CANCELED:
    case TRI_ERROR_QUERY_KILLED:
      return REQUEST_TIMEOUT;

    case TRI_ERROR_ARANGO_CONFLICT:
    case TRI_ERROR_ARANGO_UNIQUE_CONSTRAINT_VIOLATED:
    case TRI_ERROR_CURSOR_BUSY:
      return CONFLICT;

    case TRI_ERROR_ARANGO_OUT_OF_KEYS:
    case TRI_ERROR_CLUSTER_SHARD_GONE:
    case TRI_ERROR_CLUSTER_TIMEOUT:
      return SERVER_ERROR;
    
    case TRI_ERROR_CLUSTER_UNSUPPORTED:
      return NOT_IMPLEMENTED;

    case TRI_ERROR_OUT_OF_MEMORY:
    case TRI_ERROR_INTERNAL:
    default:
      return SERVER_ERROR;
  }
}

// -----------------------------------------------------------------------------
// --SECTION--                                      constructors and destructors
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief constructs a new http response
////////////////////////////////////////////////////////////////////////////////

HttpResponse::HttpResponse (HttpResponseCode code,
                            uint32_t apiCompatibility)
  : _code(code),
    _apiCompatibility(apiCompatibility),
    _isHeadResponse(false),
    _isChunked(false),
    _headers(6),
    _body(TRI_UNKNOWN_MEM_ZONE),
    _bodySize(0),
    _freeables() {

  _headers.insert(TRI_CHAR_LENGTH_PAIR("server"), "ArangoDB");
  _headers.insert(TRI_CHAR_LENGTH_PAIR("connection"), "Keep-Alive");
  _headers.insert(TRI_CHAR_LENGTH_PAIR("content-type"), "text/plain; charset=utf-8");
}

////////////////////////////////////////////////////////////////////////////////
/// @brief deletes a http response
////////////////////////////////////////////////////////////////////////////////

HttpResponse::~HttpResponse () {
  for (auto& it : _freeables) {
    delete[] it;
  }
}

// -----------------------------------------------------------------------------
// --SECTION--                                                    public methods
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief returns the response code
////////////////////////////////////////////////////////////////////////////////

HttpResponse::HttpResponseCode HttpResponse::responseCode () const {
  return _code;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief sets the response code
////////////////////////////////////////////////////////////////////////////////

void HttpResponse::setResponseCode (HttpResponseCode code) {
  _code = code;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief returns the content length
////////////////////////////////////////////////////////////////////////////////

size_t HttpResponse::contentLength () {
  if (_isHeadResponse) {
    return _bodySize;
  }

  Dictionary<char const*>::KeyValue const* kv = _headers.lookup(TRI_CHAR_LENGTH_PAIR("content-length"));

  if (kv == nullptr) {
    return 0;
  }

  return StringUtils::uint32(kv->_value);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief sets the content type
////////////////////////////////////////////////////////////////////////////////

void HttpResponse::setContentType (std::string const& contentType) {
  setHeader(TRI_CHAR_LENGTH_PAIR("content-type"), contentType);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief checks if chunked encoding is set
////////////////////////////////////////////////////////////////////////////////

bool HttpResponse::isChunked () const {
  return _isChunked;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief returns a header field
////////////////////////////////////////////////////////////////////////////////

string HttpResponse::header (std::string const& key) const {
  string k = StringUtils::tolower(key);
  Dictionary<char const*>::KeyValue const* kv = _headers.lookup(k.c_str());

  if (kv == nullptr) {
    return "";
  }
  return kv->_value;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief returns a header field
////////////////////////////////////////////////////////////////////////////////

string HttpResponse::header (char const* key, 
                             size_t keyLength) const {
  Dictionary<char const*>::KeyValue const* kv = _headers.lookup(key, keyLength);

  if (kv == nullptr) {
    return "";
  }
  return kv->_value;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief returns a header field
////////////////////////////////////////////////////////////////////////////////

string HttpResponse::header (std::string const& key, 
                             bool& found) const {
  string k = StringUtils::tolower(key);
  Dictionary<char const*>::KeyValue const* kv = _headers.lookup(k.c_str());

  if (kv == nullptr) {
    found = false;
    return "";
  }
  found = true;
  return kv->_value;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief returns a header field
////////////////////////////////////////////////////////////////////////////////

string HttpResponse::header (char const* key, 
                             size_t keyLength, 
                             bool& found) const {
  Dictionary<char const*>::KeyValue const* kv = _headers.lookup(key, keyLength);

  if (kv == nullptr) {
    found = false;
    return "";
  }
  found = true;
  return kv->_value;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief returns all header fields
////////////////////////////////////////////////////////////////////////////////

map<string, string> HttpResponse::headers () const {
  basics::Dictionary<char const*>::KeyValue const* begin;
  basics::Dictionary<char const*>::KeyValue const* end;

  map<string, string> result;

  for (_headers.range(begin, end);  begin < end;  ++begin) {
    char const* key = begin->_key;

    if (key == nullptr) {
      continue;
    }

    result[key] = begin->_value;
  }

  return result;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief sets a header field
////////////////////////////////////////////////////////////////////////////////

void HttpResponse::setHeader (char const* key, 
                              size_t keyLength, 
                              std::string const& value) {
  if (value.empty()) {
    _headers.erase(key);
  }
  else {
    char const* v = StringUtils::duplicate(value);

    if (v != nullptr) {
      _headers.insert(key, keyLength, v);
      checkHeader(key, v);

      _freeables.emplace_back(v);
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief sets a header field
////////////////////////////////////////////////////////////////////////////////

void HttpResponse::setHeader (char const* key, 
                              size_t keyLength, 
                              char const* value) {
  if (*value == '\0') {
    _headers.erase(key);
  }
  else {
    _headers.insert(key, keyLength, value);
    checkHeader(key, value);
  }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief sets a header field
////////////////////////////////////////////////////////////////////////////////

void HttpResponse::setHeader (std::string const& key, 
                              std::string const& value) {
  string lk = StringUtils::tolower(key);

  if (value.empty()) {
    _headers.erase(lk.c_str());
  }
  else {
    StringUtils::trimInPlace(lk);

    char const* k = StringUtils::duplicate(lk);
    char const* v = StringUtils::duplicate(value);

    _headers.insert(k, lk.size(), v);
    checkHeader(k, v);

    _freeables.push_back(k);
    _freeables.push_back(v);
  }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief sets many header fields
////////////////////////////////////////////////////////////////////////////////

void HttpResponse::setHeaders (std::string const& headers, 
                               bool includeLine0) {

  // make a copy we can change, this buffer will be deleted in the destructor
  char* headerBuffer = new char[headers.size() + 1];
  memcpy(headerBuffer, headers.c_str(), headers.size() + 1);

  _freeables.push_back(headerBuffer);

  // check for '\n' (we check for '\r' later)
  int lineNum = includeLine0 ? 0 : 1;

  char* start = headerBuffer;
  char* end = headerBuffer + headers.size();
  char* end1 = start;

  for (; end1 < end && *end1 != '\n';  ++end1) {
  }

  while (start < end) {
    char* end2 = end1;

    if (start < end2 && end2[-1] == '\r') {
      --end2;
    }

    // the current line is [start, end2)
    if (start < end2) {

      // now split line at the first spaces
      char* end3 = start;

      for (; end3 < end2 && *end3 != ' ' && *end3 != ':'; ++end3) {
        *end3 = ::tolower(*end3);
      }

      // the current token is [start, end3) and all in lower case
      if (lineNum == 0) {

        // the start should be HTTP/1.1 followed by blanks followed by the result code
        if (start + 8 <= end3 && memcmp(start, "http/1.1", 8) == 0) {

          char* start2 = end3;

          for (; start2 < end2 && (*start2 == ' ' || *start2 == ':');  ++start2) {
          }

          if (start2 < end2) {
            *end2 = '\0';

            _code = static_cast<HttpResponseCode>(::atoi(start2));
          }
          else {
            _code = NOT_IMPLEMENTED;
          }
        }
        else {
          _code = NOT_IMPLEMENTED;
        }
      }

      // normal header field, key is [start, end3) and the value is [start2, end4)
      else {
        for (;  end3 < end2 && *end3 != ':';  ++end3) {
          *end3 = ::tolower(*end3);
        }

        *end3 = '\0';

        if (end3 < end2) {
          char* start2 = end3 + 1;

          for (;  start2 < end2 && *start2 == ' ';  ++start2) {
          }

          char* end4 = end2;

          for (;  start2 < end4 && *(end4 - 1) == ' ';  --end4) {
          }

          *end4 = '\0';

          _headers.insert(start, end3 - start, start2);
        }
        else {
          _headers.insert(start, end3 - start, end3);
        }
          }
    }

    start = end1 + 1;

    for (end1 = start; end1 < end && *end1 != '\n';  ++end1) {
    }

    lineNum++;
  }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief add a cookie
////////////////////////////////////////////////////////////////////////////////

void HttpResponse::setCookie (std::string const& name, 
                              std::string const& value,
                              int lifeTimeSeconds, 
                              std::string const& path, 
                              std::string const& domain,
                              bool secure, 
                              bool httpOnly) {

  triagens::basics::StringBuffer* buffer = new triagens::basics::StringBuffer(TRI_UNKNOWN_MEM_ZONE);

  string tmp = StringUtils::trim(name);
  buffer->appendText(tmp.c_str(), tmp.length());
  buffer->appendChar('=');

  tmp = StringUtils::urlEncode(value);
  buffer->appendText(tmp.c_str(), tmp.length());

  if (lifeTimeSeconds != 0) {
    time_t rawtime;

    time(&rawtime);
    if (lifeTimeSeconds > 0) {
      rawtime += lifeTimeSeconds;
    }
    else {
      rawtime = 1;
    }

    if (rawtime > 0) {
      struct tm * timeinfo;
      char buffer2[80];

      timeinfo = gmtime(&rawtime);
      strftime(buffer2, 80, "%a, %d-%b-%Y %H:%M:%S %Z", timeinfo);
      buffer->appendText(TRI_CHAR_LENGTH_PAIR("; expires="));
      buffer->appendText(buffer2);
    }
  }

  if (! path.empty()) {
    buffer->appendText(TRI_CHAR_LENGTH_PAIR("; path="));
    buffer->appendText(path);
  }

  if (! domain.empty()) {
    buffer->appendText(TRI_CHAR_LENGTH_PAIR("; domain="));
    buffer->appendText(domain);
  }


  if (secure) {
    buffer->appendText(TRI_CHAR_LENGTH_PAIR("; secure"));
  }

  if (httpOnly) {
    buffer->appendText(TRI_CHAR_LENGTH_PAIR("; HttpOnly"));
  }

  char const* l = StringUtils::duplicate(buffer->c_str());
  delete buffer;
  _cookies.push_back(l);

  _freeables.push_back(l);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief swaps data
////////////////////////////////////////////////////////////////////////////////

HttpResponse* HttpResponse::swap () {
  HttpResponse* response = new HttpResponse(_code, _apiCompatibility);

  response->_headers.swap(&_headers);
  response->_body.swap(&_body);
  response->_freeables.swap(_freeables);

  bool isHeadResponse = response->_isHeadResponse;
  response->_isHeadResponse = _isHeadResponse;
  _isHeadResponse = isHeadResponse;

  size_t bodySize = response->_bodySize;
  response->_bodySize = _bodySize;
  _bodySize = bodySize;

  return response;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief writes the header
////////////////////////////////////////////////////////////////////////////////

void HttpResponse::writeHeader (StringBuffer* output) {
  bool const capitalizeHeaders = (_apiCompatibility >= 20100);

  output->appendText(TRI_CHAR_LENGTH_PAIR("HTTP/1.1 "));
  output->appendText(responseString(_code));
  output->appendText(TRI_CHAR_LENGTH_PAIR("\r\n"));

  basics::Dictionary<char const*>::KeyValue const* begin;
  basics::Dictionary<char const*>::KeyValue const* end;

  bool seenTransferEncoding = false;
  string transferEncoding;

  for (_headers.range(begin, end);  begin < end;  ++begin) {
    char const* key = begin->_key;

    if (key == nullptr) {
      continue;
    }

    size_t const keyLength = strlen(key);

    // ignore content-length
    if (keyLength == 14 && *key == 'c' && memcmp(key, "content-length", keyLength) == 0) {
      continue;
    }

    if (keyLength == 17 && *key == 't' && memcmp(key, "transfer-encoding", keyLength) == 0) {
      seenTransferEncoding = true;
      transferEncoding = begin->_value;
      continue;
    }

    if (capitalizeHeaders) {
      char const* p = key;
      char const* end = key + keyLength;
      int capState = 1;
      while (p < end) {
        if (capState == 1) {
          // upper case
          output->appendChar(::toupper(*p));
          capState = 0;
        }
        else if (capState == 0) {
          // normal case
          output->appendChar(::tolower(*p));
          if (*p == '-') {
            capState = 1;
          }
          else if (*p == ':') {
            capState = 2;
          }
        }
        else {
          // output as is
          output->appendChar(*p);
        }
        ++p;
      }
    }
    else {
      output->appendText(key, keyLength);
    }
    output->appendText(TRI_CHAR_LENGTH_PAIR(": "));
    output->appendText(begin->_value);
    output->appendText(TRI_CHAR_LENGTH_PAIR("\r\n"));
  }

  for (vector<char const*>::iterator iter = _cookies.begin();
          iter != _cookies.end(); ++iter) {
    if (capitalizeHeaders) {
      output->appendText(TRI_CHAR_LENGTH_PAIR("Set-Cookie: "));
    }
    else {
      output->appendText(TRI_CHAR_LENGTH_PAIR("set-cookie: "));
    }
    output->appendText(*iter);
    output->appendText(TRI_CHAR_LENGTH_PAIR("\r\n"));
  }

  if (seenTransferEncoding && transferEncoding == "chunked") {
    if (capitalizeHeaders) {
      output->appendText(TRI_CHAR_LENGTH_PAIR("Transfer-Encoding: chunked\r\n\r\n"));
    }
    else {
      output->appendText(TRI_CHAR_LENGTH_PAIR("transfer-encoding: chunked\r\n\r\n"));
    }
  }
  else {
    if (seenTransferEncoding) {
      if (capitalizeHeaders) {
        output->appendText(TRI_CHAR_LENGTH_PAIR("Transfer-Encoding: "));
      }
      else {
        output->appendText(TRI_CHAR_LENGTH_PAIR("transfer-encoding: "));
      }
      output->appendText(transferEncoding);
      output->appendText(TRI_CHAR_LENGTH_PAIR("\r\n"));
    }

    if (capitalizeHeaders) {
      output->appendText(TRI_CHAR_LENGTH_PAIR("Content-Length: "));
    }
    else {
      output->appendText(TRI_CHAR_LENGTH_PAIR("content-length: "));
    }

    if (_isHeadResponse) {
      // From http://www.w3.org/Protocols/rfc2616/rfc2616-sec14.html#sec14.13
      //
      // 14.13 Content-Length
      //
      // The Content-Length entity-header field indicates the size of the entity-body,
      // in decimal number of OCTETs, sent to the recipient or, in the case of the HEAD method,
      // the size of the entity-body that would have been sent had the request been a GET.
      output->appendInteger(_bodySize);
    }
    else {
      output->appendInteger(_body.length());
    }

    output->appendText(TRI_CHAR_LENGTH_PAIR("\r\n\r\n"));
  }
  // end of header, body to follow
}

////////////////////////////////////////////////////////////////////////////////
/// @brief returns the size of the body
////////////////////////////////////////////////////////////////////////////////

size_t HttpResponse::bodySize () const {
  if (_isHeadResponse) {
    return _bodySize;
  }
  return _body.length();
}

////////////////////////////////////////////////////////////////////////////////
/// @brief returns the body
////////////////////////////////////////////////////////////////////////////////

StringBuffer& HttpResponse::body () {
  return _body;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief indicates a head response
////////////////////////////////////////////////////////////////////////////////

void HttpResponse::headResponse (size_t size) {
  _body.clear();
  _isHeadResponse = true;
  _bodySize = size;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief deflates the response body
///
/// the body must already be set. deflate is then run on the existing body
////////////////////////////////////////////////////////////////////////////////

int HttpResponse::deflate (size_t bufferSize) {
  int res = _body.deflate(bufferSize);

  if (res != TRI_ERROR_NO_ERROR) {
    return res;
  }

  setHeader(TRI_CHAR_LENGTH_PAIR("content-encoding"), "deflate");
  return TRI_ERROR_NO_ERROR;
}

// -----------------------------------------------------------------------------
// --SECTION--                                                   private methods
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief checks for special headers
////////////////////////////////////////////////////////////////////////////////

void HttpResponse::checkHeader (const char* key, const char* value) {
  if (key[0] == 't' && strcmp(key, "transfer-encoding") == 0) {
    if (TRI_CaseEqualString(value, "chunked")) {
      _isChunked = true;
    }
    else {
      _isChunked = false;
    }
  }
}

// -----------------------------------------------------------------------------
// --SECTION--                                                       END-OF-FILE
// -----------------------------------------------------------------------------

// Local Variables:
// mode: outline-minor
// outline-regexp: "/// @brief\\|/// {@inheritDoc}\\|/// @page\\|// --SECTION--\\|/// @\\}"
// End:
