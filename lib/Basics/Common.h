////////////////////////////////////////////////////////////////////////////////
/// @brief High-Performance Database Framework made by triagens
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
/// @author Copyright 2009-2013, triAGENS GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

#ifndef ARANGODB_BASICS_COMMON_H
#define ARANGODB_BASICS_COMMON_H 1

// -----------------------------------------------------------------------------
// --SECTION--                                             configuration options
// -----------------------------------------------------------------------------

#define TRI_WITHIN_COMMON 1
#include "Basics/operating-system.h"
#ifdef _WIN32
#include "Basics/local-configuration-win.h"
#else
#include "Basics/local-configuration.h"
#endif
#include "Basics/application-exit.h"

#include "build.h"

#ifdef _DEBUG
#define TRI_VERSION_FULL TRI_VERSION " [" TRI_PLATFORM "-DEBUG]"
#else
#define TRI_VERSION_FULL TRI_VERSION " [" TRI_PLATFORM "]"
#endif

#undef TRI_WITHIN_COMMON

// -----------------------------------------------------------------------------
// --SECTION--                                                    C header files
// -----------------------------------------------------------------------------

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <math.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef TRI_HAVE_POSIX_THREADS
#ifdef _GNU_SOURCE
#include <pthread.h>
#else
#define _GNU_SOURCE
#include <pthread.h>
#undef _GNU_SOURCE
#endif
#endif

#ifdef TRI_HAVE_PROCESS_H
#include <process.h>
#endif

#ifdef TRI_HAVE_SIGNAL_H
#include <signal.h>
#endif

#ifdef TRI_HAVE_STDBOOL_H
#include <stdbool.h>
#endif

#ifdef TRI_HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef TRI_HAVE_STRINGS_H
#include <strings.h>
#endif

#include <sys/stat.h>
#include <sys/types.h>

#ifdef TRI_HAVE_SYS_RESOURCE_H
#include <sys/resource.h>
#endif

#ifdef TRI_HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

// .............................................................................
// The problem we have for visual studio is that if we include WinSock2.h here
// it may conflict later in some other source file. The conflict arises when
// windows.h is included BEFORE WinSock2.h -- this is a visual studio issue. For
// now be VERY careful to ensure that if you need windows.h, then you include
// this file AFTER common.h.
// .............................................................................

#ifdef TRI_HAVE_WINSOCK2_H
#include <WinSock2.h>
typedef long suseconds_t;
#endif

// -----------------------------------------------------------------------------
// --SECTION--           C++ header files that are always present on all systems
// -----------------------------------------------------------------------------

#include <algorithm>
#include <deque>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <fstream>
#include <stack>
#include <string>
#include <vector>

// C++11!!
#include <array>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <thread>

// -----------------------------------------------------------------------------
// --SECTION--                                            basic triAGENS headers
// -----------------------------------------------------------------------------

#define TRI_WITHIN_COMMON 1
#include "Basics/voc-errors.h"
#include "Basics/error.h"
#include "Basics/debugging.h"
#include "Basics/memory.h"
#include "Basics/mimetypes.h"
#include "Basics/structures.h"
#undef TRI_WITHIN_COMMON

// -----------------------------------------------------------------------------
// --SECTION--                                              basic compiler stuff
// -----------------------------------------------------------------------------

#define TRI_WITHIN_COMMON 1
#include "Basics/system-compiler.h"
#include "Basics/system-functions.h"
#undef TRI_WITHIN_COMMON

// -----------------------------------------------------------------------------
// --SECTION--                                                 low level helpers
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief incrementing a uint64_t modulo a number with wraparound
////////////////////////////////////////////////////////////////////////////////

static inline uint64_t TRI_IncModU64 (uint64_t i, uint64_t len) {
  // Note that the dummy variable gives the compiler a (good) chance to
  // use a conditional move instruction instead of a branch. This actually
  // works on modern gcc.
  uint64_t dummy;
  dummy = (++i) - len;
  return i < len ? i : dummy;
}

static inline uint64_t TRI_DecModU64 (uint64_t i, uint64_t len) {
  if ((i--) != 0) {
    return i;
  }
  return len-1;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief a trivial hash function for uint64_t to uint32_t
////////////////////////////////////////////////////////////////////////////////

static inline uint32_t TRI_64to32 (uint64_t x) {
  return static_cast<uint32_t>(x >> 32) ^ static_cast<uint32_t>(x);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief helper macro for calculating strlens for static strings at 
/// a compile-time (unless compiled with fno-builtin-strlen etc.)
////////////////////////////////////////////////////////////////////////////////

#define TRI_CHAR_LENGTH_PAIR(value) (value), strlen(value)

////////////////////////////////////////////////////////////////////////////////
/// @brief fake spinlocks
/// spin locks seem to have issues when used under Valgrind
/// we thus mimic spinlocks using ordinary mutexes when in maintainer mode
////////////////////////////////////////////////////////////////////////////////

#ifdef TRI_ENABLE_MAINTAINER_MODE

#define TRI_FAKE_SPIN_LOCKS 1

#ifndef TRI_ASSERT
#define TRI_ASSERT(expr) { if (! (expr)) TRI_PrintBacktrace(); assert(expr); }
#define TRI_ASSERT_EXPENSIVE(expr) { if (! (expr)) TRI_PrintBacktrace(); assert(expr); }
#endif


#else


#undef TRI_FAKE_SPIN_LOCKS

#ifndef TRI_ASSERT
#define TRI_ASSERT(expr) (static_cast<void>(0))
#define TRI_ASSERT_EXPENSIVE(expr) (static_cast<void>(0))
#endif

#endif


#ifdef _WIN32
#include "Basics/win-utils.h"
#endif

// -----------------------------------------------------------------------------
// --SECTIONS--                                                          alignas
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief struct alignas(x) ... does not work in Visual Studio 2013
////////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
#define TRI_ALIGNAS(x)
#else
#define TRI_ALIGNAS(x) alignas(x)
#endif

// -----------------------------------------------------------------------------
// --SECTIONS--                                               deferred execution
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// Use in a function (or scope) as:
///   TRI_DEFER( <ONE_STATEMENT> );
/// and the statement will be called regardless if the function throws or
/// returns regularly.
/// Do not put multiple TRI_DEFERs on a single source code line (will not
/// compile).
/// Multiple TRI_DEFERs in one scope will be executed in reverse order of
/// appearance.
/// The idea to this is from
///   http://blog.memsql.com/c-error-handling-with-auto/
////////////////////////////////////////////////////////////////////////////////

#define TOKEN_PASTE_WRAPPED(x, y) x ## y
#define TOKEN_PASTE(x, y) TOKEN_PASTE_WRAPPED(x, y)

template<typename T>
struct TRI_AutoOutOfScope {
    TRI_AutoOutOfScope(T& destructor) : m_destructor(destructor) { }
    ~TRI_AutoOutOfScope() { m_destructor(); }
  private:
    T& m_destructor;
};

#define TRI_DEFER_INTERNAL(Destructor, funcname, objname) \
  auto funcname = [&]() { Destructor; }; \
  TRI_AutoOutOfScope<decltype(funcname)> objname(funcname);

#define TRI_DEFER(Destructor) TRI_DEFER_INTERNAL(Destructor, TOKEN_PASTE(auto_fun, __LINE__) , TOKEN_PASTE(auto_obj, __LINE__))

// -----------------------------------------------------------------------------
// --SECTIONS--                                               triagens namespace
// -----------------------------------------------------------------------------

namespace triagens {
//  using namespace std;

  typedef TRI_blob_t blob_t;
  typedef TRI_datetime_t datetime_t;
  typedef TRI_date_t date_t;
  typedef TRI_seconds_t seconds_t;
  typedef TRI_msec_t msec_t;
}

#undef TRI_SHOW_LOCK_TIME 
#define TRI_SHOW_LOCK_THRESHOLD 0.000199

#endif

// -----------------------------------------------------------------------------
// --SECTION--                                                       END-OF-FILE
// -----------------------------------------------------------------------------

// Local Variables:
// mode: outline-minor
// outline-regexp: "/// @brief\\|/// {@inheritDoc}\\|/// @page\\|// --SECTION--\\|/// @\\}"
// End:
