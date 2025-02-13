////////////////////////////////////////////////////////////////////////////////
/// @brief implementation of basis class LineEditor
///
/// @file
///
/// DISCLAIMER
///
/// Copyright 2014-2015 ArangoDB GmbH, Cologne, Germany
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
/// @author Copyright 2014-2015, ArangoDB GmbH, Cologne, Germany
/// @author Copyright 2011-2013, triAGENS GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

#include "LineEditor.h"

#include "Utilities/ShellImplementation.h"
#include "Utilities/Completer.h"

using namespace std;
using namespace triagens;
using namespace arangodb;

// -----------------------------------------------------------------------------
// --SECTION--                                                  class LineEditor
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// --SECTION--                                      constructors and destructors
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief constructs a new editor
////////////////////////////////////////////////////////////////////////////////

LineEditor::LineEditor (std::string const& history)
  : _history(history) {
  _shellImpl = nullptr;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief destructor
////////////////////////////////////////////////////////////////////////////////

LineEditor::~LineEditor () {
  if (_shellImpl) {
    close();
    delete _shellImpl;
  }
 }

// -----------------------------------------------------------------------------
// --SECTION--                                              static public methods
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief sort the alternatives results vector
////////////////////////////////////////////////////////////////////////////////

void LineEditor::sortAlternatives (vector<string>& completions) {
  std::sort(completions.begin(), completions.end(),
    [](std::string const& l, std::string const& r) -> bool {
      int res = strcasecmp(l.c_str(), r.c_str());
      return (res < 0);
    }
  );
}

// -----------------------------------------------------------------------------
// --SECTION--                                                    public methods
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief line editor open
////////////////////////////////////////////////////////////////////////////////

bool LineEditor::open (bool autoComplete) {
  prepareShell();
  return _shellImpl->open(autoComplete);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief line editor shutdown
////////////////////////////////////////////////////////////////////////////////

bool LineEditor::close () {
  prepareShell();
  return _shellImpl->close();
}

////////////////////////////////////////////////////////////////////////////////
/// @brief line editor prompt
////////////////////////////////////////////////////////////////////////////////

string LineEditor::prompt (const string& prompt,
			   const string& begin,
			   bool& eof) {
  return _shellImpl->prompt(prompt, begin, eof);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief get the history file path
////////////////////////////////////////////////////////////////////////////////

string LineEditor::historyPath () {
  prepareShell();
  return _shellImpl->historyPath();
}

////////////////////////////////////////////////////////////////////////////////
/// @brief add to history
////////////////////////////////////////////////////////////////////////////////

void LineEditor::addHistory (const string& line) {
  prepareShell();
  return _shellImpl->addHistory(line);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief save history
////////////////////////////////////////////////////////////////////////////////

bool LineEditor::writeHistory () {
  prepareShell();
  return _shellImpl->writeHistory();
}

////////////////////////////////////////////////////////////////////////////////
/// @brief send a signal to the shell implementation
////////////////////////////////////////////////////////////////////////////////

void LineEditor::signal () {
  _shellImpl->signal();
}

// -----------------------------------------------------------------------------
// --SECTION--                                                 protected methods
// -----------------------------------------------------------------------------


void LineEditor::prepareShell () {
  if (! _shellImpl) {
    initializeShell();
  }
}
// -----------------------------------------------------------------------------
// --SECTION--                                                       END-OF-FILE
// -----------------------------------------------------------------------------
