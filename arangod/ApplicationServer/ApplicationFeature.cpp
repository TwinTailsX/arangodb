////////////////////////////////////////////////////////////////////////////////
/// @brief application server feature
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
/// @author Copyright 2014, ArangoDB GmbH, Cologne, Germany
/// @author Copyright 2010-2014, triAGENS GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

#include "ApplicationFeature.h"

using namespace triagens;
using namespace triagens::rest;
using namespace std;

// -----------------------------------------------------------------------------
// --SECTION--                                      constructors and destructors
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief constructor
////////////////////////////////////////////////////////////////////////////////

ApplicationFeature::ApplicationFeature (string const& name)
  : _disabled(false), 
    _name(name) {
}

////////////////////////////////////////////////////////////////////////////////
/// @brief destructor
////////////////////////////////////////////////////////////////////////////////

ApplicationFeature::~ApplicationFeature () {
}

// -----------------------------------------------------------------------------
// --SECTION--                                                    public methods
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief returns the name
////////////////////////////////////////////////////////////////////////////////

string const& ApplicationFeature::getName () const {
  return _name;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief sets up the options
////////////////////////////////////////////////////////////////////////////////

void ApplicationFeature::setupOptions (map<string, basics::ProgramOptionsDescription>&) {
}

////////////////////////////////////////////////////////////////////////////////
/// @brief callback after options parsing and config file reading, 
/// before dropping privileges
////////////////////////////////////////////////////////////////////////////////

bool ApplicationFeature::afterOptionParsing (basics::ProgramOptions&) {
  return true;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief prepares the feature
////////////////////////////////////////////////////////////////////////////////

bool ApplicationFeature::prepare () {
  return true;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief prepares the feature
////////////////////////////////////////////////////////////////////////////////

bool ApplicationFeature::prepare2 () {
  return true;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief starts the feature
////////////////////////////////////////////////////////////////////////////////

bool ApplicationFeature::start () {
  return true;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief opens the feature for business
////////////////////////////////////////////////////////////////////////////////

bool ApplicationFeature::open () {
  return true;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief closes the feature
////////////////////////////////////////////////////////////////////////////////

void ApplicationFeature::close () {
}

////////////////////////////////////////////////////////////////////////////////
/// @brief stops everything
////////////////////////////////////////////////////////////////////////////////

void ApplicationFeature::stop () {
}

////////////////////////////////////////////////////////////////////////////////
/// @brief disable feature
////////////////////////////////////////////////////////////////////////////////

void ApplicationFeature::disable () {
  _disabled = true;
}

// -----------------------------------------------------------------------------
// --SECTION--                                                       END-OF-FILE
// -----------------------------------------------------------------------------

// Local Variables:
// mode: outline-minor
// outline-regexp: "/// @brief\\|/// {@inheritDoc}\\|/// @page\\|// --SECTION--\\|/// @\\}"
// End:
