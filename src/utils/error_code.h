// NOLINT(llvm-header-guard)
#pragma once

#include <system_error>

#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

static inline std::error_code get_net_error_code() {
#ifdef _WIN32
  return {WSAGetLastError(), std::system_category()};
#else
  return {errno, std::system_category()};
#endif
}

static inline std::error_code get_error_code() {
#ifdef _WIN32
  return {(int)GetLastError(), std::system_category()};
#else
  return {errno, std::system_category()};
#endif
}

#define THROW_EC(ec) throw std::system_error(ec)
