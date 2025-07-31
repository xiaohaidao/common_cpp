
#include "gtest/gtest.h"

#include "coroutine/coroutine.h"
#include "proactor/Proactor.h"
#include "proactor/ProactorCo.h"
#include "sockets/TcpListener.h"
#include "utils/log.h"

namespace proactor_co {

bool stop_task = false;

void co_close(socket_type s, const char *module) {
  LOG_DEBUG("module: %s, close socket %d", module, s);
  std::error_code ec;
  tcp_stream(s).close(ec);
  if (!stop_task)
    EXPECT_FALSE(ec) << "module: " << module << ", " << ec.value() << " : "
                     << ec.message();
}

void async_read(socket_type s, const char *module);
void connected(const char *module, const socket_addr &addr) {
  std::error_code ec;
  socket_type const s = co_connect(addr, ec);
  EXPECT_FALSE(ec) << "module: " << module << ", " << addr.get_ip() << ":"
                   << addr.get_port() << ", " << ec.value() << " : "
                   << ec.message();
  ec.clear();
  LOG_DEBUG("%s: connect %s:%d complete", module, addr.get_ip(),
            addr.get_port());

  async_read(s, module);
}

void async_connect(const char *module, const socket_addr &addr) {
  CO_AWAIT([&addr, module]() { connected(module, addr); });
}

void co_write(socket_type s, const char *module, const char *buff, size_t size);
void read(socket_type s, const char *module) {
  LOG_DEBUG("[%s] %d begin read", module, s);
  char buff[1024] = {};
  std::error_code ec;
  if (co_tcp_read(s, buff, sizeof(buff), ec) <= 0) {
    co_close(s, module);
    return;
  }
  EXPECT_FALSE(ec) << "module: " << module << ", " << ec.value() << " : "
                   << ec.message();
  LOG_DEBUG("[%s] read \"%s\"", module, buff);

  if (ec) {
    return;
  }
  co_write(s, module, buff, (size_t)strlen(buff));
}

void async_read(socket_type s, const char *module) {
  CO_AWAIT([s, module]() { read(s, module); });
}

void co_write(socket_type s, const char *module, const char *buff,
              size_t size) {
  LOG_DEBUG("[%s] %d write message (%d)%s", module, s, size, buff);
  std::error_code ec;
  co_tcp_write(s, buff, size, ec);
  EXPECT_FALSE(ec) << "module: " << module << ", " << ec.value() << " : "
                   << ec.message();
  LOG_DEBUG("[%s] write message completely (%d)%s", module, size, buff);
  if (ec) {
    return;
  }
  async_read(s, module);
}

class service {
public:
  service() = default;
  ~service() = default;

  socket_type native() const { return listener_.native(); }

  void close() {
    for (auto &i : tcps_) {
      std::error_code ec;
      tcp_stream(i).close(ec);
      if (!stop_task)
        EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
    }
    LOG_DEBUG("server close socket %d", native());
    std::error_code ec;
    listener_.close(ec);
    EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();

    exit_task_ = true;
  }

  void bind(const char *port) {
    std::error_code ec;
    listener_ = listener_.bind(port, ec);
    EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
    ec.clear();
  }

  void accept() {
    while (!stop_task) {
      if (exit_task_) {
        break;
      }
      std::error_code ec;
      socket_addr from = {};
      socket_type const new_socket = co_accept(listener_.native(), from, ec);
      if (new_socket == 0) {
        break;
      }
      EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
      LOG_DEBUG("socket %d client ip %d and port %s:%d", native(), new_socket,
                from.get_ip(), from.get_port());
      if (ec) {
        continue;
      }
      tcps_.push_back(new_socket);
      co_write(new_socket, "server", "Server send message", 19);
      async_read(new_socket, "server");
    }
  }

  void async_accept() {
    CO_AWAIT([this]() { accept(); });
  }

private:
  tcp_listener listener_;
  std::vector<socket_type> tcps_;
  bool exit_task_{false};
};

} // namespace proactor_co

TEST(ProactorTest, ProactorCo) {
  using namespace proactor_co;
  std::error_code ec;
  proactor p(ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  set_proactor(&p);

  socket_addr const addr(nullptr, "8989");
  LOG_DEBUG("local ip is %s port %d", addr.get_ip(), addr.get_port());
  char port[8] = {};
  snprintf(port, sizeof(port), "%d", addr.get_port());

  service server;
  LOG_DEBUG("bind port %s", port);
  server.bind(port);
  LOG_DEBUG("server accept");
  server.async_accept();

  LOG_DEBUG("client async connect");
  async_connect("client", addr);

  LOG_DEBUG("-------------------- begin run while --------------------");
  size_t i = 0;
  CO_LOOP_CALL([&p, &ec, &i]() {
    p.run_one(0, ec);
    EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
    ec.clear();
    if (++i > 10) {
      stop_task = true;
      set_proactor(nullptr);
      p.shutdown();
    }
  });
  LOG_DEBUG("-------------------- end run while --------------------");
  server.close();

  p.close(ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
}
