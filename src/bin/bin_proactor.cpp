
#include <stdio.h>

#include <memory>

#include "proactor/Proactor.h"
#include "proactor/operation/TcpListenerOp.h"
#include "proactor/operation/TcpStreamOp.h"

class Client : public std::enable_shared_from_this<Client> {
  char buff_[1024];
  TcpStreamOp tcp_op_;

public:
  Client(const TcpStreamOp &op) : tcp_op_(op) {
    std::error_code ec;
    sockets::setKeepLive(tcp_op_.native_handle(), ec, 1, 10, 1, 3);
    if (ec) {
      fprintf(stderr, "set keep live error %d:%s\n", ec.value(),
              ec.message().c_str());
    }
  }

  ~Client() {
    std::error_code ec;
    auto address = SocketAddr::get_remote_socket(tcp_op_.native_handle(), ec);
    tcp_op_.close(ec);
    printf("close remote client %s:%d\n", address.get_ip(), address.get_port());
  }

  void write(size_t length) {
    auto self = shared_from_this();
    std::error_code ec;
    tcp_op_.async_write(
        buff_, length,
        [this, self](const std::error_code &re, size_t size) {
          if (!re) {
            read();
          } else {
            std::error_code ec;
            auto address =
                SocketAddr::get_remote_socket(tcp_op_.native_handle(), ec);
            printf("remote client close %s:%d re %s\n", address.get_ip(),
                   address.get_port(), re.message().c_str());
          }
        },
        ec);
  }

  void read() {
    auto self = shared_from_this();
    std::error_code ec;
    tcp_op_.async_read(
        buff_, sizeof(buff_),
        [this, self](const std::error_code &re, size_t size) {
          if (!re && size != 0) {
            write(size);
          } else {
            std::error_code ec;
            auto address =
                SocketAddr::get_remote_socket(tcp_op_.native_handle(), ec);
            printf("remote client close %s:%d re %s\n", address.get_ip(),
                   address.get_port(), re.message().c_str());
          }
        },
        ec);
  }
};

class Server {

  TcpListenerOp server_;

  void do_accept() {
    std::error_code ec;
    server_.async_accept(
        [this](const std::error_code &re,
               const std::pair<TcpStreamOp, SocketAddr> &client) {
          if (!re) {
            printf("connect remote %s:%d\n", client.second.get_ip(),
                   client.second.get_port());
            std::make_shared<Client>(client.first)->read();
          }
          do_accept();
        },
        ec);
    if (ec) {
      fprintf(stderr, "async_accept error %s\n", ec.message().c_str());
    }
  }

public:
  Server(Proactor &p) : server_(p) {
    std::error_code ec;
    server_.bind("8088", ec);
    if (ec) {
      fprintf(stderr, "bind 8088 error %s\n", ec.message().c_str());
    }
    do_accept();
  }
  ~Server() {
    std::error_code ec;
    server_.close(ec);
    printf("close server\n");
  }
};

int main(int args, char **argv) {
  std::error_code ec;
  Proactor a(ec);
  if (ec) {
    fprintf(stderr, "Proactor create error %s\n", ec.message().c_str());
  }

  Server server(a);

  a.run();
  return 0;
}
