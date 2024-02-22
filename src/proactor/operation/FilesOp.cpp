
#include "proactor/operation/FilesOp.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <fcntl.h>
#include <unistd.h>
#endif

#include "utils/error_code.h"

FilesOp::FilesOp() : ctx_(nullptr), fd_(0) {}

FilesOp::FilesOp(Proactor *context) : ctx_(context), fd_(0) {}

FilesOp::FilesOp(Proactor *context, ::native_handle s) : ctx_(context), fd_(s) {

#ifdef _WIN32
  if (ctx_ != nullptr) {
    std::error_code ec;
    ctx_->post((HANDLE)fd_, nullptr, ec); // register to io proactor
    // if (ec) {
    //   LOG_WARN("overlapped post error %d %s", ec.value(), ec.message());
    // }
  }
#endif
}

FilesOp::FilesOp(const FilesOp &other) : ctx_(other.ctx_), fd_(other.fd_) {}

FilesOp &FilesOp::operator=(const FilesOp &other) {
  if (&other == this) {
    return *this;
  }
  this->ctx_ = other.ctx_;
  this->fd_ = other.fd_;
  // this->read_op_ = detail::ReadOp();
  // this->write_op_ = detail::WriteOp();
  return *this;
}

/** don't support file
void FilesOp::open(const char *file_path, std::error_code &ec) {
  open(file_path, false, ec);
}

void FilesOp::open(const char *file_path, bool create, std::error_code &ec) {
#ifdef _WIN32
  DWORD num = 0;
  fd_ = ::CreateFile(file_path, GENERIC_READ | GENERIC_WRITE,
                     FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                     create ? OPEN_ALWAYS : OPEN_EXISTING, FILE_FLAG_OVERLAPPED,
                     nullptr);
  if (fd_ == INVALID_HANDLE_VALUE) {
    std::error_code re_ec = get_error_code();
    if (re_ec.value() != ERROR_ALREADY_EXISTS) {
      ec = re_ec;
      fd_ = 0;
    }
  }
#else
  fd_ = ::open(file_path, O_RDWR | (create ? O_CREAT : 0),
               S_IRWXU | S_IRWXG | S_IRWXO);
  if (fd_ == -1) {
    ec = get_error_code();
    fd_ = 0;
  }
#endif
}
*/

size_t FilesOp::read(char *buff, size_t buff_size, std::error_code &ec) {
#ifdef _WIN32
  DWORD num = 0;
  if (!::ReadFile(fd_, buff, static_cast<DWORD>(buff_size), &num, NULL)) {
    ec = get_error_code();
  }
  return num;
#else
  int num = ::read(fd_, buff, buff_size);
  if (num == -1) {
    ec = get_error_code();
    num = 0;
  }
  return num;
#endif
}

size_t FilesOp::write(const char *buff, size_t buff_size, std::error_code &ec) {
#ifdef _WIN32
  DWORD num = 0;
  if (!::WriteFile(fd_, buff, static_cast<DWORD>(buff_size), &num, NULL)) {
    ec = get_error_code();
  }
  return num;
#else
  int num = ::write(fd_, buff, buff_size);
  if (num == -1) {
    ec = get_error_code();
    num = 0;
  }
  return num;
#endif
}

void FilesOp::async_read(char *buff, size_t buff_size, const func_type &f,
                         std::error_code &ec) {

  auto call_back = [f](void * /*ctx*/, const std::error_code &re_ec,
                       size_t recv_size) { f(re_ec, recv_size); };

  read_op_.async_read(ctx_, call_back, fd_, buff, buff_size, ec);
}

void FilesOp::async_write(const char *buff, size_t buff_size,
                          const func_type &f, std::error_code &ec) {

  auto call_back = [f](void * /*ctx*/, const std::error_code &re_ec,
                       size_t send_size) { f(re_ec, send_size); };

  write_op_.async_write(ctx_, call_back, fd_, buff, buff_size, ec);
}

void FilesOp::close(std::error_code &ec) {
  if (ctx_) {
    std::error_code t_ec;
    ctx_->cancel((::native_handle)fd_, t_ec);
  }
#ifdef _WIN32
  if (!::CloseHandle(fd_)) {
    ec = get_error_code();
  }
#else
  if (::close(fd_) == -1) {
    ec = get_error_code();
  }
#endif
  fd_ = 0;
}

native_handle FilesOp::native() const { return fd_; }
