#include <stdio.h>

#include "ipc/PipeListener.h"
#include "ipc/PipeStream.h"
#include "utils/log.h"

int main(int args, char **argv, char * /*env*/[]) {
  if (args < 2) {
    printf("Usage:\n"
           "    %s <cmd> [-s|-c]\n",
           argv[0]);
    return -1;
  }

#define MACOR_FUN()                                                            \
  do {                                                                         \
    printf(">");                                                               \
    fscanf(stdin, "%s", buff);                                                 \
    if (strcmp(buff, "exit") == 0) {                                           \
      break;                                                                   \
    }                                                                          \
    pip.write(buff, strlen(buff), ec);                                         \
    if (ec) {                                                                  \
      printf("write pipe error %s\n", ec.message().c_str());                   \
    }                                                                          \
    memset(buff, 0, sizeof(buff));                                             \
    pip.read(buff, sizeof(buff), ec);                                          \
    if (ec) {                                                                  \
      printf("read pipe error %s\n", ec.message().c_str());                    \
    }                                                                          \
    printf("recv %s\n", buff);                                                 \
  } while (true);                                                              \
                                                                               \
  pip.close(ec);                                                               \
  if (ec) {                                                                    \
    printf("close pipe error %s\n", ec.message().c_str());                     \
  }

  constexpr char kPipeName[] = "test_pipe";
  std::error_code ec;
  char buff[4096];
  if (strcmp(argv[1], "-s") == 0) {
    auto pip_s = ipc::PipeListener::create(kPipeName, ec);
    if (ec) {
      printf("create pipe error %s\n", ec.message().c_str());
      ec.clear();
    }
    auto pip = pip_s.accept(ec);
    if (ec) {
      printf("accpet pipe error %s\n", ec.message().c_str());
      return -1;
    }
    printf("accpet pipe success\n");

    MACOR_FUN()
    pip_s.remove(ec);
    if (ec) {
      printf("pipe remove error %s\n", ec.message().c_str());
    }
  } else if (strcmp(argv[1], "-c") == 0) {
    auto pip = ipc::PipeStream::connect(kPipeName, ec);
    if (ec) {
      printf("connect pipe error %s\n", ec.message().c_str());
      return -1;
    }
    // MACOR_FUN()
    do {
      memset(buff, 0, sizeof(buff));
      pip.read(buff, sizeof(buff), ec);
      if (ec) {
        printf("read pipe error %s\n", ec.message().c_str());
      }
      int const s = (int)strlen(buff);
      printf("recv %d %s\n", s, buff);
      char b[1024];
      snprintf(b, sizeof(b), "%.*s%s", s, buff, " from client\n");
      pip.write(b, strlen(b), ec);
      if (ec) {
        printf("write pipe error %s\n", ec.message().c_str());
      }
    } while (true);
  }

  return 0;
}
