
#ifndef UTILS_ERROR_CODE_H
#define UTILS_ERROR_CODE_H

#include <system_error>

#ifdef WIN32
#include <windows.h>
#endif // WIN32

inline std::error_code getErrorCode() {
#ifdef WIN32
  return {(int)GetLastError(), std::system_category()};
#else
  return {errno, std::system_category()};
#endif
}

#endif // UTILS_ERROR_CODE_H
