
#include <stdio.h>

#include <cstring>
#include <memory>

#include "proactor/Proactor.h"
#include "proactor/operation/IpcListenerOp.h"
#include "proactor/operation/IpcStreamOp.h"
#include "utils/log.h"

class Client : public std::enable_shared_from_this<Client> {
  char buff_[1024];
  IpcStreamOp tcp_op_;

public:
  Client(const IpcStreamOp &op) : tcp_op_(op) {}

  ~Client() {
    std::error_code ec;
    tcp_op_.close(ec);
    LOG_INFO("%d close remote client %s\n", tcp_op_.native(),
             ec.message().c_str());
  }

  void connect(const char *path) {
    std::error_code ec;
    tcp_op_.connect(path, ec);
    if (ec) {
      LOG_INFO("%d connect error %s\n", tcp_op_.native(), ec.message().c_str());
    }
  }

  void write(const char *str) {
    int r = snprintf(buff_, sizeof(buff_), "%s", str);
    LOG_INFO("r %d:%zu\n", r, strlen(str));
    write(r);
  }

  void write(size_t length) {
    auto self = shared_from_this();
    std::error_code ec;
    tcp_op_.async_write(buff_, length,
                        [this, self](const std::error_code &re, size_t size) {
                          if (!re) {
                            LOG_INFO("%d write: success size %d\n",
                                     tcp_op_.native(), size);
                            read();
                          } else {
                            LOG_INFO("%d write error %s\n", tcp_op_.native(),
                                     re.message().c_str());
                          }
                        },
                        ec);

    if (ec) {
      LOG_INFO("%d write error %s\n", tcp_op_.native(), ec.message().c_str());
    }
  }

  void read() {
    auto self = shared_from_this();
    std::error_code ec;
    tcp_op_.async_read(buff_, sizeof(buff_),
                       [this, self](const std::error_code &re, size_t size) {
                         if (!re && size != 0) {
                           buff_[size] = 0;
                           LOG_INFO("%d read(%d): %*s\n", tcp_op_.native(),
                                    size, (int)size, buff_);
                           write(size);
                         } else {
                           std::error_code ec;
                           LOG_INFO("remote client close %d re %s\n",
                                    tcp_op_.native(), re.message().c_str());
                         }
                       },
                       ec);
    if (ec) {
      LOG_INFO("%d read error %s\n", tcp_op_.native(), ec.message().c_str());
    }
  }
};

class Server {

  IpcListenerOp server_;

  void do_accept() {
    std::error_code ec;
    server_.async_accept(
        [this](const std::error_code &re, const IpcStreamOp &client) {
          if (!re) {
            LOG_INFO("connect remote %d\n", client.native());
            std::make_shared<Client>(client)->read();
          } else {
            LOG_INFO("async connect error remote %d er %d\n", client.native(),
                     re.value());
          }
          do_accept();
        },
        ec);
    if (ec) {
      LOG_ERROR("async_accept error %s\n", ec.message().c_str());
    }
  }

public:
  Server(Proactor &p, const char *port) : server_(p) {
    std::error_code ec;
    server_.bind(port, ec);
    if (ec) {
      LOG_ERROR("bind %s error %d %s\n", port, ec.value(),
                ec.message().c_str());
    }
    do_accept();
  }

  ~Server() {
    std::error_code ec;
    server_.close(ec);
    LOG_INFO("close server\n");
  }
};

int main(int args, char **argv) {
  if (args != 3) {
    printf("Usage:\n"
           "    %s [-s|-c] <name>\n",
           argv[0]);
    return -1;
  }
  if (strcmp(argv[1], "-c") == 0) {
    std::error_code ec;
    Proactor a(ec);
    if (ec) {
      LOG_ERROR("Proactor create error %s\n", ec.message().c_str());
    }

    IpcStreamOp op(&a);
    auto client = std::make_shared<Client>(op);
    client->connect(argv[2]);
    client->write("client send message!");
    a.run();

  } else if (strcmp(argv[1], "-s") == 0) {
    std::error_code ec;
    Proactor a(ec);
    if (ec) {
      LOG_ERROR("Proactor create error %s\n", ec.message().c_str());
    }

    Server server(a, argv[2]);

    a.run();
  }

  return 0;
}
