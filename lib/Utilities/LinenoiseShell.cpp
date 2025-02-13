////////////////////////////////////////////////////////////////////////////////
/// @brief console input using linenoise
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
/// @author Copyright 2011-2013, triAGENS GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

#include "LinenoiseShell.h"

extern "C" {
#include <linenoise.h>
}

#include "Utilities/Completer.h"
#include "Utilities/LineEditor.h"
#include "Basics/files.h"

using namespace std;
using namespace triagens;
using namespace arangodb;

// -----------------------------------------------------------------------------
// --SECTION--                                                 private functions
// -----------------------------------------------------------------------------

namespace arangodb {
  Completer* COMPLETER;
}

static void LinenoiseCompletionGenerator (char const* text, 
					  linenoiseCompletions* lc) {
  if (COMPLETER) {
    std::vector<string> alternatives = COMPLETER->alternatives(text);
    arangodb::LineEditor::sortAlternatives(alternatives);

    for (auto& it : alternatives) {
      linenoiseAddCompletion(lc, it.c_str());
    }
  }

  lc->multiLine = 1;
}

// -----------------------------------------------------------------------------
// --SECTION--                                              class LinenoiseShell
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// --SECTION--                                      constructors and destructors
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief constructs a new editor
////////////////////////////////////////////////////////////////////////////////

LinenoiseShell::LinenoiseShell (std::string const& history, 
                                Completer* completer)
: ShellImplementation(history, completer) {
  COMPLETER = completer;
  linenoiseSetCompletionCallback(LinenoiseCompletionGenerator);
}

LinenoiseShell::~LinenoiseShell() {
  COMPLETER = nullptr;
}

// -----------------------------------------------------------------------------
// --SECTION--                                                    public methods
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// {@inheritDoc}
////////////////////////////////////////////////////////////////////////////////

bool LinenoiseShell::open (bool) {
  linenoiseHistoryLoad(historyPath().c_str());

  _state = STATE_OPENED;

  return true;
}

////////////////////////////////////////////////////////////////////////////////
/// {@inheritDoc}
////////////////////////////////////////////////////////////////////////////////

bool LinenoiseShell::close () {
  if (_state != STATE_OPENED) {
    // avoid duplicate saving of history
    return true;
  }

  _state = STATE_CLOSED;

  return writeHistory();
}

////////////////////////////////////////////////////////////////////////////////
/// {@inheritDoc}
////////////////////////////////////////////////////////////////////////////////

string LinenoiseShell::historyPath () {
  string path;

  // get home directory
  char* p = TRI_HomeDirectory();

  if (p != nullptr) {
    path.append(p);
    TRI_Free(TRI_CORE_MEM_ZONE, p);

    if (! path.empty() && path[path.size() - 1] != TRI_DIR_SEPARATOR_CHAR) {
      path.push_back(TRI_DIR_SEPARATOR_CHAR);
    }
  }

  path.append(_historyFilename);

  return path;
}

////////////////////////////////////////////////////////////////////////////////
/// {@inheritDoc}
////////////////////////////////////////////////////////////////////////////////

void LinenoiseShell::addHistory (std::string const& str) {
  if (str.empty()) {
    return;
  }

  linenoiseHistoryAdd(str.c_str());
}

////////////////////////////////////////////////////////////////////////////////
/// {@inheritDoc}
////////////////////////////////////////////////////////////////////////////////

bool LinenoiseShell::writeHistory () {
  linenoiseHistorySave(historyPath().c_str());

  return true;
}

////////////////////////////////////////////////////////////////////////////////
/// {@inheritDoc}
////////////////////////////////////////////////////////////////////////////////

std::string LinenoiseShell::getLine(const std::string& prompt, bool& eof) {
  eof = false;
  return linenoise(prompt.c_str());
}

// -----------------------------------------------------------------------------
// --SECTION--                                                       END-OF-FILE
// -----------------------------------------------------------------------------

// Local Variables:
// mode: outline-minor
// outline-regexp: "/// @brief\\|/// {@inheritDoc}\\|/// @page\\|// --SECTION--\\|/// @\\}"
// End:
