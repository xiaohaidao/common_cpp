
#ifdef __linux__

#include "ipc/PipeStream.h"

#include <cstring>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "utils/error_code.h"
#include "utils/random.h"

namespace ipc {

PipeStream::PipeStream() : named_pipe_(0) {}

PipeStream PipeStream::connect(const char *name_pipe, std::error_code &ec) {
  PipeStream re;
  int server = ::open(name_pipe, O_RDWR);
  if (server == -1) {
    ec = get_error_code();
    return re;
  }
  re.named_pipe_ = server;
  re.name_ = name_pipe;
  return re;
}

size_t PipeStream::read(char *buff, size_t buff_size, std::error_code &ec) {
  int num = ::read(named_pipe_, buff, buff_size);
  if (num == -1) {
    ec = get_error_code();
    num = 0;
  }
  return num;
}

size_t PipeStream::write(const char *buff, size_t buff_size,
                         std::error_code &ec) {
  int num = ::write(named_pipe_, buff, buff_size);
  if (num == -1) {
    ec = get_error_code();
    num = 0;
  }
  return num;
}

void PipeStream::close(std::error_code &ec) {
  if (::close(named_pipe_) == -1) {
    ec = get_error_code();
  }
}

native_handle PipeStream::native() const { return named_pipe_; }

} // namespace ipc

#endif // __linux__
