
#ifdef __linux__

#include "ipc/Pipe.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "utils/error_code.h"

namespace ipc {

Pipe::Pipe() : read_pipe_(0), write_pipe_(0), error_pipe_(0) {}

Pipe Pipe::std_in_out() {
  Pipe re;
  re.read_pipe_ = STDIN_FILENO;
  re.write_pipe_ = STDOUT_FILENO;
  re.error_pipe_ = STDERR_FILENO;
  return re;
}

Pipe Pipe::create(std::error_code &ec) {
  Pipe re;

  int pipefd[2];
  if (::pipe(pipefd) == -1) {
    ec = getErrorCode();
    return re;
  }

  re.read_pipe_ = pipefd[0];
  re.write_pipe_ = pipefd[1];
  re.error_pipe_ = re.read_pipe_;
  return re;
}

Pipe Pipe::create(const char *name_pipe, std::error_code &ec) {
  if (::mkfifo(name_pipe, 0666) == -1) {
    ec = getErrorCode();
    return Pipe();
  }

  return connect(name_pipe, ec);
}

Pipe Pipe::connect(const char *name_pipe, std::error_code &ec) {
  Pipe re;
  int server = ::open(name_pipe, O_RDWR);
  if (server == -1) {
    ec = getErrorCode();
    return re;
  }
  re.read_pipe_ = server;
  re.write_pipe_ = server;
  re.error_pipe_ = re.read_pipe_;
  re.name_ = name_pipe;
  return re;
}

size_t Pipe::read(char *buff, size_t buff_size, std::error_code &ec) {
  int num = ::read(read_pipe_, buff, buff_size);
  if (num == -1) {
    ec = getErrorCode();
    num = 0;
  }
  return num;
}

size_t Pipe::write(const char *buff, size_t buff_size, std::error_code &ec) {
  int num = ::write(write_pipe_, buff, buff_size);
  if (num == -1) {
    ec = getErrorCode();
    num = 0;
  }
  return num;
}

void Pipe::close(std::error_code &ec) {
  if (::close(read_pipe_) == -1) {
    ec = getErrorCode();
  }
  if (read_pipe_ != write_pipe_) {
    if (::close(write_pipe_) == -1) {
      ec = getErrorCode();
    }
  }
  if (!name_.empty() && ::unlink(name_.c_str()) == -1) {
    std::error_code re_ec = getErrorCode();
    if (re_ec.value() != ENOENT) {
      ec = re_ec;
    }
  }
}

native_handle Pipe::read_native() const { return read_pipe_; }

native_handle Pipe::write_native() const { return write_pipe_; }

native_handle Pipe::error_native() const { return error_pipe_; }

} // namespace ipc

#endif // __linux__
