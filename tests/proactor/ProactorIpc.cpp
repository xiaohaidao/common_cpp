
#include "gtest/gtest.h"

#include <cstring>
#include <memory>

#include "proactor/Proactor.h"
#include "proactor/operation/IpcListenerOp.h"
#include "proactor/operation/IpcStreamOp.h"
#include "utils/log.h"

class client : public std::enable_shared_from_this<client> {
  char buff_[1024];
  ipc_stream_op tcp_op_;

public:
  client(const ipc_stream_op &op) : tcp_op_(op) {}

  ~client() {
    std::error_code ec;
    tcp_op_.close(ec);
    LOG_INFO("~ %d close remote client %s\n", tcp_op_.native(),
             ec.message().c_str());
  }

  void close() {
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
    int const r = snprintf(buff_, sizeof(buff_), "%s", str);
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
                           std::error_code const ec;
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

class server {

  ipc_listener_op server_;

  void do_accept() {
    std::error_code ec;
    server_.async_accept(
        [this](const std::error_code &re, const ipc_stream_op &c) {
          if (!re) {
            LOG_INFO("connect remote %d\n", c.native());
            std::make_shared<client>(c)->read();
          } else {
            LOG_INFO("async connect error remote %d er %d\n", c.native(),
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
  server(proactor &p, const char *port) : server_(p) {
    std::error_code ec;
    server_.bind(port, ec);
    if (ec) {
      LOG_ERROR("bind %s error %d %s\n", port, ec.value(),
                ec.message().c_str());
    }
    do_accept();
  }

  ~server() {
    std::error_code ec;
    server_.close(ec);
    LOG_INFO("close server\n");
  }
};

TEST(ProactorTest, IpcOp) {
  std::error_code ec;
  proactor p(ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  const char name[] = "gen_server";
  server const server(p, name);

  ipc_stream_op const op(&p);
  auto c = std::make_shared<client>(op);
  c->connect(name);
  c->write("client send message!");

  LOG_DEBUG("-------------------- begin run while --------------------");
  for (size_t i = 0; i < 10; ++i) {
    p.run_one(1000ull * 1000ull, ec);
    EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
    ec.clear();
  }
  LOG_DEBUG("-------------------- end run while --------------------");

  c->close();
  for (size_t i = 0; i < 2; ++i) {
    p.run_one(1000ull * 1000ull, ec);
    EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
    ec.clear();
  }

  p.close(ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
}
