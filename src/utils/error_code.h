
#ifndef UTILS_ERROR_CODE_H
#define UTILS_ERROR_CODE_H

#include <system_error>

#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

static inline std::error_code getNetErrorCode() {
#ifdef _WIN32
  return {WSAGetLastError(), std::system_category()};
#else
  return {errno, std::system_category()};
#endif
}

static inline std::error_code getErrorCode() {
#ifdef _WIN32
  return {(int)GetLastError(), std::system_category()};
#else
  return {errno, std::system_category()};
#endif
}

#define THROW_EC(ec) throw std::system_error(ec)

#endif // UTILS_ERROR_CODE_H
