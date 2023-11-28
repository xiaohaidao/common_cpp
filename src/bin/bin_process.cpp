#include <stdio.h>

#include "process/Process.h"
#include "utils/log.h"

int main(int args, char **argv, char *env[]) {
  if (args < 2) {
    printf("Usage:\n"
           "    %s <cmd> [args]\n",
           argv[0]);
    return -1;
  }

  std::error_code ec;
  std::vector<const char *> argsv;
  for (int i = 2; i < args; ++i) {
    argsv.emplace_back(argv[i]);
    LOG_TRACE("argsv emplace back: %s", argv[i]);
  }
  Process p = Process::call(argv[1], argsv, ec);
  // Process p = Process::call("cmd", {"/c echo 3 && pause"}, ec);
  if (ec) {
    LOG_ERROR("call process error %d:%s", ec.value(), ec.message().c_str());
    return -1;
  }
  p.wait(ec);
  if (ec) {
    LOG_ERROR("process wait error %d:%s", ec.value(), ec.message().c_str());
    return -1;
  }
  return 0;
}
