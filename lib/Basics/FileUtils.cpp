////////////////////////////////////////////////////////////////////////////////
/// @brief file utilities
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
/// @author Copyright 2008-2013, triAGENS GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

#include "FileUtils.h"

#ifdef TRI_HAVE_DIRENT_H
#include <dirent.h>
#endif

#ifdef TRI_HAVE_DIRECT_H
#include <direct.h>
#endif

#include "Basics/Exceptions.h"
#include "Basics/StringBuffer.h"
#include "Basics/files.h"
#include "Basics/logging.h"
#include "Basics/tri-strings.h"

#if defined(_WIN32) && defined(_MSC_VER)

#define TRI_DIR_FN(item)                item.name

#else

#define TRI_DIR_FN(item)                item->d_name

#endif

using namespace std;

namespace triagens {
  namespace basics {
    namespace FileUtils {

      void throwFileReadError (int fd, std::string const& filename) {
        TRI_set_errno(TRI_ERROR_SYS_ERROR);
        int res = TRI_errno();

        if (fd >= 0) {
          TRI_CLOSE(fd);
        }

        std::string message("read failed for file '" + filename + "': " + strerror(res));
        LOG_TRACE("%s", message.c_str());

        THROW_ARANGO_EXCEPTION(TRI_ERROR_SYS_ERROR);
      }
      
      void throwFileWriteError (int fd, std::string const& filename) {
        TRI_set_errno(TRI_ERROR_SYS_ERROR);
        int res = TRI_errno();

        if (fd >= 0) {
          TRI_CLOSE(fd);
        }

        std::string message("write failed for file '" + filename + "': " + strerror(res));
        LOG_TRACE("%s", message.c_str());

        THROW_ARANGO_EXCEPTION(TRI_ERROR_SYS_ERROR);
      }

      string slurp (string const& filename) {
        int fd = TRI_OPEN(filename.c_str(), O_RDONLY);

        if (fd == -1) {
          throwFileReadError(fd, filename);
        }

        char buffer[10240];
        StringBuffer result(TRI_CORE_MEM_ZONE);

        while (true) {
          ssize_t n = TRI_READ(fd, buffer, sizeof(buffer));

          if (n == 0) {
            break;
          }

          if (n < 0) {
            throwFileReadError(fd, filename);
          }

          result.appendText(buffer, n);
        }

        TRI_CLOSE(fd);

        string r(result.c_str(), result.length());

        return r;
      }



      void slurp (string const& filename, StringBuffer& result) {
        int fd = TRI_OPEN(filename.c_str(), O_RDONLY);

        if (fd == -1) {
          throwFileReadError(fd, filename);
        }

        // reserve space in the output buffer
        off_t fileSize = size(filename);
        if (fileSize > 0) {
          result.reserve((size_t) fileSize);
        }

        char buffer[10240];

        while (true) {
          ssize_t n = TRI_READ(fd, buffer, sizeof(buffer));

          if (n == 0) {
            break;
          }

          if (n < 0) {
            throwFileReadError(fd, filename);
          }

          result.appendText(buffer, n);
        }

        TRI_CLOSE(fd);
      }


      void spit (string const& filename, const char* ptr, size_t len) {
        int fd = TRI_CREATE(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP);

        if (fd == -1) {
          throwFileWriteError(fd, filename);
        }

        while (0 < len) {
          ssize_t n = TRI_WRITE(fd, ptr, (TRI_write_t) len);

          if (n < 1) {
            throwFileWriteError(fd, filename);
          }

          ptr += n;
          len -= n;
        }

        TRI_CLOSE(fd);
      }

      void spit (string const& filename, string const& content) {
        int fd = TRI_CREATE(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP);

        if (fd == -1) {
          throwFileWriteError(fd, filename);
        }

        char const* ptr = content.c_str();
        size_t len = content.size();

        while (0 < len) {
          ssize_t n = TRI_WRITE(fd, ptr, (TRI_write_t) len);

          if (n < 1) {
            throwFileWriteError(fd, filename);
          }

          ptr += n;
          len -= n;
        }

        TRI_CLOSE(fd);
      }



      void spit (string const& filename, StringBuffer const& content) {
        int fd = TRI_CREATE(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP);

        if (fd == -1) {
          throwFileWriteError(fd, filename);
        }

        char const* ptr = content.c_str();
        size_t len = content.length();

        while (0 < len) {
          ssize_t n = TRI_WRITE(fd, ptr, (TRI_write_t) len);

          if (n < 1) {
            throwFileWriteError(fd, filename);
          }

          ptr += n;
          len -= n;
        }

        TRI_CLOSE(fd);
      }


      bool remove (string const& fileName, int* errorNumber) {
        if (errorNumber != nullptr) {
          *errorNumber = 0;
        }

        int result = std::remove(fileName.c_str());

        if (errorNumber != nullptr) {
          *errorNumber = errno;
        }

        return (result != 0) ? false : true;
      }



      bool rename (string const& oldName, string const& newName, int* errorNumber) {
        if (errorNumber != nullptr) {
          *errorNumber = 0;
        }

        int result = std::rename(oldName.c_str(), newName.c_str());

        if (errorNumber != nullptr) {
          *errorNumber = errno;
        }

        return (result != 0) ? false : true;
      }



      bool createDirectory (string const& name, int* errorNumber) {
        if (errorNumber != nullptr) {
          *errorNumber = 0;
        }

        return createDirectory(name, 0777, errorNumber);
      }



      bool createDirectory (string const& name, int mask, int* errorNumber) {
        if (errorNumber != nullptr) {
          *errorNumber = 0;
        }

        int result = TRI_MKDIR(name.c_str(), mask);

        int res = errno;
        if (result != 0 && res == EEXIST && isDirectory(name)) {
          result = 0;
        }
        else if (res != 0) {
          TRI_set_errno(TRI_ERROR_SYS_ERROR);
        }

        if (errorNumber != nullptr) {
          *errorNumber = res;
        }

        return (result != 0) ? false : true;
      }

      bool copyRecursive (std::string const& source, std::string const& target, std::string& error) {
        if (isDirectory(source)) {
          return copyDirectoryRecursive (source, target, error);
        }

        return TRI_CopyFile(source, target, error);
      }

      bool copyDirectoryRecursive (std::string const& source, std::string const& target, std::string& error) {
        bool rc = true;
#ifdef TRI_HAVE_WIN32_LIST_FILES
        auto isSymlink = [] (struct _finddata_t item) -> bool {
          return false;
        };

        auto isSubDirectory = [] (struct _finddata_t item) -> bool {
          return ((item.attrib & _A_SUBDIR) != 0);
        };

        struct _finddata_t oneItem;
        intptr_t handle;

        string filter = source + "\\*";
        handle = _findfirst(filter.c_str(), &oneItem);

        if (handle == -1) {
          error = "directory " + source + "not found";
          return false;
        }

        do {
#else
        auto isSymlink = [] (struct dirent* item) -> bool {
          return (item->d_type == DT_LNK);
        };

        auto isSubDirectory = [] (struct dirent* item) -> bool {
          return (item->d_type == DT_DIR);
        };

        struct dirent* d = (struct dirent*) TRI_Allocate(TRI_UNKNOWN_MEM_ZONE, (offsetof(struct dirent, d_name) + PATH_MAX + 1), false); 

        if (d == nullptr) {
          error = "directory " + source + " OOM";
          return false;
        }

        DIR* filedir = opendir(source.c_str());

        if (filedir == nullptr) {
          TRI_Free(TRI_UNKNOWN_MEM_ZONE, d);
          error = "directory " + source + "not found";
          return false;
        }

        struct dirent* oneItem;
        while ((readdir_r(filedir, d, &oneItem) == 0) &&
               (oneItem != nullptr)) {
#endif
          // Now iterate over the items.
          // check its not the pointer to the upper directory:
          if (! strcmp(TRI_DIR_FN(oneItem), ".") ||
              ! strcmp(TRI_DIR_FN(oneItem), "..")) {
            continue;
          }

          std::string dst = target + TRI_DIR_SEPARATOR_STR + TRI_DIR_FN(oneItem);
          std::string src = source + TRI_DIR_SEPARATOR_STR + TRI_DIR_FN(oneItem);
            
          // Handle subdirectories:
          if (isSubDirectory(oneItem)) {
            long systemError;
            int rc = TRI_CreateDirectory(dst.c_str(), systemError, error);
            if (rc != TRI_ERROR_NO_ERROR) {
              break;
            }
            if (! copyDirectoryRecursive(src, dst, error)) {
              break;
            }
            if (! TRI_CopyAttributes(src, dst, error)) {
              break;
            }
          }
          else if (isSymlink(oneItem)) {
            if (! TRI_CopySymlink(src, dst, error)) {
              break;
            }
          }
          else {
            if (! TRI_CopyFile(src, dst, error)) {
              break;
            }
          }
#ifdef TRI_HAVE_WIN32_LIST_FILES
        } 
        while (_findnext(handle, &oneItem) != -1);

        _findclose(handle);

#else
        }
        TRI_Free(TRI_UNKNOWN_MEM_ZONE, d);
        closedir(filedir);

#endif
        return rc;
      }


      vector<string> listFiles (string const& directory) {
        vector<string> result;

#ifdef TRI_HAVE_WIN32_LIST_FILES

        struct _finddata_t fd;
        intptr_t handle;

        string filter = directory + "\\*";
        handle = _findfirst(filter.c_str(), &fd);

        if (handle == -1) {
          return result;
        }

        do {
          if (strcmp(fd.name, ".") != 0 && strcmp(fd.name, "..") != 0) {
            result.push_back(fd.name);
          }
        } 
        while (_findnext(handle, &fd) != -1);

        _findclose(handle);

#else

        DIR* d = opendir(directory.c_str());

        if (d == nullptr) {
          return result;
        }

        dirent* de = readdir(d);

        while (de != nullptr) {
          if (strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0) {
            result.push_back(de->d_name);
          }

          de = readdir(d);
        }

        closedir(d);

#endif

        return result;
      }



      bool isDirectory (string const& path) {
        TRI_stat_t stbuf;
        int res = TRI_STAT(path.c_str(), &stbuf);
        return (res == 0) && ((stbuf.st_mode & S_IFMT) == S_IFDIR);
      }



      bool isSymbolicLink (string const& path) {

#ifdef TRI_HAVE_WIN32_SYMBOLIC_LINK

        // .........................................................................
        // TODO: On the NTFS file system, there are the following file links:
        // hard links -
        // junctions -
        // symbolic links -
        // .........................................................................
        return false;

#else

        struct stat stbuf;
        int res = TRI_STAT(path.c_str(), &stbuf);

        return (res == 0) && ((stbuf.st_mode & S_IFMT) == S_IFLNK);

#endif
      }



      bool isRegularFile (string const& path) {
        TRI_stat_t stbuf;
        int res = TRI_STAT(path.c_str(), &stbuf);
        return (res == 0) && ((stbuf.st_mode & S_IFMT) == S_IFREG);
      }



      bool exists (string const& path) {
        TRI_stat_t stbuf;
        int res = TRI_STAT(path.c_str(), &stbuf);

        return res == 0;
      }


      off_t size (string const& path) {
        int64_t result = TRI_SizeFile(path.c_str());

        if (result < 0) {
          return (off_t) 0;
        }

        return (off_t) result;
      }


      string stripExtension (string const& path, string const& extension) {
        size_t pos = path.rfind(extension);
        if (pos == string::npos) {
          return path;
        }

        string last = path.substr(pos);
        if (last == extension) {
          return path.substr(0, pos);
        }

        return path;
      }



      bool changeDirectory (string const& path) {
        return TRI_CHDIR(path.c_str()) == 0;
      }



      string currentDirectory (int* errorNumber) {
        if (errorNumber != 0) {
          *errorNumber = 0;
        }

        size_t len = 1000;
        char* current = new char[len];

        while (TRI_GETCWD(current, (int) len) == nullptr) {
          if (errno == ERANGE) {
            len += 1000;
            delete[] current;
            current = new char[len];
          }
          else {
            delete[] current;

            if (errorNumber != 0) {
              *errorNumber = errno;
            }

            return ".";
          }
        }

        string result = current;

        delete[] current;

        return result;
      }


      string homeDirectory () {
        char* dir = TRI_HomeDirectory();
        string result = dir;
        TRI_FreeString(TRI_CORE_MEM_ZONE, dir);

        return result;
      }
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
