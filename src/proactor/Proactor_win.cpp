
#ifdef _WIN32

#include "proactor/Proactor.h"

#include <windows.h>

#include "utils/error_code.h"
#include "utils/macro.h"

Proactor::Proactor() : fd_(0), shutdown_(false) {}

Proactor Proactor::create(std::error_code &ec) {
  Proactor p;
  CHECK_EC(ec, p);

  HANDLE han = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);

  if (han == nullptr) {
    ec = getErrorCode();
  }
  p.fd_ = han;
}

void Proactor::shutdown() {
  shutdown_ = true;

  std::error_code ec;
  notify_op(nullptr, ec);
}

size_t Proactor::run() {
  std::error_code ec;
  size_t n;
  while (run_one(INFINITE, ec)) {
    if (n <= *std::numeric_limits<size_t>::max) {
      ++n;
    }
  }
  return n;
}

size_t Proactor::run_one(size_t timeout_us, std::error_code &ec) {
  for (; !shutdown_;) {
    DWORD bytes_transferred = 0;
    ULONG_PTR completion_key = 0;
    LPOVERLAPPED overlapped = nullptr;
    BOOL ok = GetQueuedCompletionStatus(
        fd_, &bytes_transferred, &completion_key, &overlapped, timeout_us);
    // timeout_us < gqcs_timeout_ ? timeout_us : gqcs_timeout_);

    std::error_code result_ec = getErrorCode();
    if (overlapped) {
      auto op = static_cast<Operation *>(overlapped);

      op->complete(this, result_ec, bytes_transferred);

      return 1;
    } else if (!ok) {
      ec = result_ec;
      return 0;
    }
  }
  return 0;
}

void Proactor::notify_op(Operation *op, std::error_code &ec) {
  if (!PostQueuedCompletionStatus(fd_, 0, 0, op)) {
    ec = getErrorCode();
  }
}

void Proactor::post(const native_handle &file_descriptor, std::error_code &ec) {
  HANDLE han = CreateIoCompletionPort(file_descriptor, fd_, 0, 0);
  if (han == nullptr) {
    ec = getErrorCode();
    return;
  }
  // assert(han == fd_);
}

#endif // _WIN32
