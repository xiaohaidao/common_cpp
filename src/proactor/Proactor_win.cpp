
#ifdef _WIN32

#include "proactor/Proactor.h"

#include <limits>
#include <windows.h>

#include "utils/error_code.h"

struct ThreadInfo {};

Proactor::Proactor() : fd_(nullptr), shutdown_(false) {}

Proactor::Proactor(std::error_code &ec) {
  HANDLE han = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);
  if (han == nullptr) {
    ec = getErrorCode();
  }
  fd_ = han;
}

void Proactor::shutdown() {
  shutdown_ = true;

  std::error_code ec;
  notify_op(nullptr, ec); // TODO: multiple thread close
}

size_t Proactor::run() {
  std::error_code ec;
  size_t n = 0;
  ThreadInfo thread_info;
  while (call_one(INFINITE, thread_info, ec)) {
    if (n <= (std::numeric_limits<size_t>::max)()) {
      ++n;
    }
  }
  return n;
}

size_t Proactor::run_one(size_t timeout_us, std::error_code &ec) {
  ThreadInfo thread_info;
  return call_one(timeout_us, thread_info, ec);
}

void Proactor::notify_op(Operation *op, std::error_code &ec) {
  if (!PostQueuedCompletionStatus(fd_, 0, 0, op)) {
    ec = getErrorCode();
  }
}

void Proactor::post(native_handle file_descriptor, Operation * /*op*/,
                    std::error_code &ec) {
  HANDLE han = CreateIoCompletionPort(file_descriptor, fd_, 0, 0);
  if (han == nullptr) {
    ec = getErrorCode();
    return;
  }
  // assert(han == fd_);
}

void Proactor::cancel(native_handle file_descriptor, std::error_code &ec) {

  if (!::CancelIoEx(file_descriptor, nullptr)) {
    std::error_code re_ec = getErrorCode();
    // if (re_ec.value() != ERROR_NOT_FOUND) {
    ec = re_ec;
    // }
  }
}

void Proactor::close(std::error_code &ec) {
  if (!::CloseHandle(fd_)) {
    ec = getErrorCode();
  }
}

size_t Proactor::call_one(size_t timeout_us, ThreadInfo &thread_info,
                          std::error_code &ec) {

  for (; !shutdown_;) {
    DWORD bytes_transferred = 0;
    ULONG_PTR completion_key = 0;
    LPOVERLAPPED overlapped = nullptr;
    BOOL ok =
        GetQueuedCompletionStatus(fd_, &bytes_transferred, &completion_key,
                                  &overlapped, timeout_us / 1000);
    // timeout_us < gqcs_timeout_ ? timeout_us : gqcs_timeout_);

    std::error_code result_ec = getErrorCode();
    if (result_ec.value() == ERROR_IO_PENDING) {
      result_ec = {0, result_ec.category()};
    }
    if (overlapped) {
      auto op = static_cast<Operation *>(overlapped);

      op->complete(this, result_ec, bytes_transferred);

      return 1;
    } else if (!ok) {
      if (result_ec.value() != WAIT_TIMEOUT) {
        ec = result_ec;
      }
      return 0;
    }
  }
  return 0;
}

#endif // _WIN32
