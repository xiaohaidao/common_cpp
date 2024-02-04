
#ifdef __linux__

#include "ipc/PipeListener.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "utils/error_code.h"

namespace ipc {

PipeListener::PipeListener() : named_pipe_(0) {}

PipeListener PipeListener::create(const char *name_pipe, std::error_code &ec) {
  PipeListener re;
  if (::mkfifo(name_pipe, 0666) == -1) {
    ec = get_error_code();
  }
  re.name_ = name_pipe;
  return re;
}

void PipeListener::accept(std::error_code &ec) {
  int server = ::open(name_.c_str(), O_RDWR);
  if (server == -1) {
    ec = get_error_code();
  } else {
    named_pipe_ = server;
  }
}

size_t PipeListener::read(char *buff, size_t buff_size, std::error_code &ec) {
  int num = ::read(named_pipe_, buff, buff_size);
  if (num == -1) {
    ec = get_error_code();
    num = 0;
  }
  return num;
}

size_t PipeListener::write(const char *buff, size_t buff_size,
                           std::error_code &ec) {
  int num = ::write(named_pipe_, buff, buff_size);
  if (num == -1) {
    ec = get_error_code();
    num = 0;
  }
  return num;
}

void PipeListener::close(std::error_code &ec) {
  if (::close(named_pipe_) == -1) {
    ec = get_error_code();
  }
}

void PipeListener::remove(std::error_code &ec) {
  if (!name_.empty() && named_pipe_ > 0 && ::unlink(name_.c_str()) == -1) {
    std::error_code re_ec = get_error_code();
    if (re_ec.value() != ENOENT) {
      ec = re_ec;
    }
  }
}

native_handle PipeListener::native() const { return named_pipe_; }

} // namespace ipc

#endif // __linux__
