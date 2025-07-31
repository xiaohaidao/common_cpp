
#ifdef __linux__

#include "ipc/Pipe.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "utils/error_code.h"

namespace ipc {

pipe::pipe() = default;

pipe pipe::std_in_out() {
  pipe re;
  re.read_pipe_ = STDIN_FILENO;
  re.write_pipe_ = STDOUT_FILENO;
  re.error_pipe_ = STDERR_FILENO;
  return re;
}

pipe pipe::create(std::error_code &ec) {
  pipe re;

  int pipefd[2];
  if (::pipe(pipefd) == -1) {
    ec = get_error_code();
    return re;
  }

  re.read_pipe_ = pipefd[0];
  re.write_pipe_ = pipefd[1];
  re.error_pipe_ = re.read_pipe_;
  return re;
}

size_t pipe::read(char *buff, size_t buff_size, std::error_code &ec) {
  int num = ::read(read_pipe_, buff, buff_size);
  if (num == -1) {
    ec = get_error_code();
    num = 0;
  }
  return num;
}

size_t pipe::write(const char *buff, size_t buff_size, std::error_code &ec) {
  int num = ::write(write_pipe_, buff, buff_size);
  if (num == -1) {
    ec = get_error_code();
    num = 0;
  }
  return num;
}

void pipe::close(std::error_code &ec) {
  if (::close(read_pipe_) == -1) {
    ec = get_error_code();
  }
  if (::close(write_pipe_) == -1) {
    ec = get_error_code();
  }
}

native_handle pipe::read_native() const { return read_pipe_; }

native_handle pipe::write_native() const { return write_pipe_; }

native_handle pipe::error_native() const { return error_pipe_; }

} // namespace ipc

#endif // __linux__
