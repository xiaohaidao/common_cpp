
#ifdef __linux__

#include "ipc/PipeListener.h"

#include <cstring>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "utils/error_code.h"

namespace ipc {

PipeListener::PipeListener() : name_{} {}

PipeListener PipeListener::create(const char *name_pipe, std::error_code &ec) {
  PipeListener re;
  if (::mkfifo(name_pipe, 0666) == -1) {
    ec = get_error_code();
  }
  snprintf(re.name_, sizeof(re.name_), "%s", name_pipe);
  return re;
}

void PipeListener::create(std::error_code &ec) {
  if (::mkfifo(name_, 0666) == -1) {
    ec = get_error_code();
  }
}

PipeStream PipeListener::accept(std::error_code &ec) {
  PipeStream re;
  int server = ::open(name_, O_RDWR);
  if (server == -1) {
    ec = get_error_code();
  } else {
    return PipeStream(server);
  }
  return re;
}

void PipeListener::remove(std::error_code &ec) {
  if (strlen(name_) != 0 && ::unlink(name_) == -1) {
    std::error_code re_ec = get_error_code();
    if (re_ec.value() != ENOENT) {
      ec = re_ec;
    }
  }
}

native_handle PipeListener::native() const { return 0; }

} // namespace ipc

#endif // __linux__
