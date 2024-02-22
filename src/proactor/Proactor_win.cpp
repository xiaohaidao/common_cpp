
#ifdef _WIN32

#include "proactor/Proactor.h"

#include <limits>
#include <windows.h>

#include "utils/error_code.h"

struct ThreadInfo {
  QueueOp queue;
};

Proactor::Proactor() : fd_(nullptr), shutdown_(false) {}

Proactor::Proactor(std::error_code &ec) : fd_(nullptr), shutdown_(false) {
  HANDLE han = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);
  if (han == nullptr) {
    ec = get_error_code();
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
  while (!shutdown_) {
    if (call_one(INFINITE, thread_info, ec) &&
        n <= (std::numeric_limits<size_t>::max)()) {
      ++n;
    }
  }
  shutdown();
  return n;
}

size_t Proactor::run_one(size_t timeout_us, std::error_code &ec) {
  ThreadInfo thread_info;
  size_t n = call_one(timeout_us, thread_info, ec);
  while (thread_info.queue.begin()) {
    if (call_one(timeout_us, thread_info, ec)) {
      ++n;
    }
  }
  return n;
}

void Proactor::notify_op(Operation *op, std::error_code &ec) {
  if (!PostQueuedCompletionStatus(fd_, 0, 0, op)) {
    ec = get_error_code();
  }
}

void Proactor::post(native_handle file_descriptor, Operation * /*op*/,
                    std::error_code &ec) {
  HANDLE han = CreateIoCompletionPort(file_descriptor, fd_, 0, 0);
  if (han == nullptr) {
    ec = get_error_code();
    return;
  }
  // assert(han == fd_);
}

void Proactor::cancel(native_handle file_descriptor, std::error_code &ec) {

  if (!::CancelIoEx(file_descriptor, nullptr)) {
    std::error_code const re_ec = get_error_code();
    // if (re_ec.value() != ERROR_NOT_FOUND) {
    ec = re_ec;
    // }
  }
}

void Proactor::close(std::error_code &ec) {
  if (!::CloseHandle(fd_)) {
    ec = get_error_code();
  }
}

size_t Proactor::call_one(size_t timeout_us, ThreadInfo &thread_info,
                          std::error_code &ec) {
  QueueOp &queue = thread_info.queue;
  for (; !shutdown_;) {
    if (!queue.empty()) {
      auto *op = static_cast<Operation *>(queue.begin());
      queue.pop();
      op->complete(this, std::error_code(), 0);
      return 1;
    }
    size_t timeout_ms = timer_queue_.wait_duration_ms(INT32_MAX);
    if (timeout_ms <= 0) {
      std::lock_guard<std::mutex> const lck(timer_mutex_);
      timer_queue_.get_all_task(queue);
    }
    if (!queue.empty()) {
      continue;
    }
    timeout_ms = (std::min)(timeout_ms, timeout_us / 1000);
    DWORD bytes_transferred = 0;
    ULONG_PTR completion_key = 0;
    LPOVERLAPPED overlapped = nullptr;
    BOOL const ok =
        GetQueuedCompletionStatus(fd_, &bytes_transferred, &completion_key,
                                  &overlapped, (DWORD)timeout_ms);

    std::error_code result_ec = get_error_code();
    if (result_ec.value() == ERROR_IO_PENDING ||
        result_ec.value() == WAIT_TIMEOUT) {
      result_ec = {0, result_ec.category()};
    }
    if (overlapped) {
      auto *op = static_cast<Operation *>(overlapped);

      op->complete(this, result_ec, bytes_transferred);

      return 1;
    }
    if (!ok) {
      if (result_ec.value() != WAIT_TIMEOUT) {
        ec = result_ec;
      }
      return 0;
    }
  }
  return 0;
}

#endif // _WIN32
