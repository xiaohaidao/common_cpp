
#include "gtest/gtest.h"

#include <map>

#include "reactor/Reactor.h"
#include "sockets/SocketAddr.h"
#include "sockets/UdpSocket.h"
#include "utils/log.h"

template <typename T> class Udp : public ReactorOp {
public:
  Udp() = default;
  explicit Udp(const char *module) : module_(module) {}

  void close() {
    LOG_TRACE("module: %s, close socket %d", module_.c_str(), native_handle());
    std::error_code ec;
    client_.close(ec);
    EXPECT_FALSE(ec) << "module: " << module_ << ", " << ec.value() << " : "
                     << ec.message();
    ec.clear();
  }

  socket_type native_handle() const { return client_.native_handle(); }

  void bind(const char *port) {
    std::error_code ec;
    client_ = UdpSocket::bind(port, ec);
    EXPECT_FALSE(ec) << "module: " << module_ << ", " << ec.value() << " : "
                     << ec.message();
    ec.clear();
  }

  void connect(const SocketAddr &addr) {
    to_ = addr;
    std::error_code ec;
    client_ = UdpSocket::create(kIpV4, ec);
    EXPECT_FALSE(ec) << "module: " << module_ << ", " << ec.value() << " : "
                     << ec.message();
    ec.clear();
  }

  void write(const char *buff, size_t buff_size) {
    LOG_TRACE("module: %s, write to %s:%d message %d, \"%s\"", module_.c_str(),
              to_.get_ip(), to_.get_port(), buff_size, buff);
    std::error_code ec;
    client_.send_to(buff, buff_size, to_, ec);
    EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
    ec.clear();
  }

  void read(void *reactor) {
    LOG_TRACE("module: %s, socket %d begin read", module_.c_str(),
              native_handle());
    std::error_code ec;
    std::pair<size_t, SocketAddr> recv =
        client_.recv_from(buff_, sizeof(buff_), ec);
    EXPECT_FALSE(ec) << "module: " << module_ << ", " << ec.value() << " : "
                     << ec.message();
    ec.clear();
    size_t n = recv.first;
    buff_[n] = 0;
    LOG_TRACE("module: %s, client read from %s:%d buff %d \"%s\"",
              module_.c_str(), recv.second.get_ip(), recv.second.get_port(), n,
              buff_);

    if (n == 0) {
      LOG_TRACE("module: %s, close socket %d", module_.c_str(),
                client_.native_handle());
      client_.close(ec);
      EXPECT_FALSE(ec) << "module: " << module_ << ", " << ec.value() << " : "
                       << ec.message();
      ec.clear();

      return;
    }

    SocketAddr &to = recv.second;
    LOG_TRACE("module: %s, client write to %s:%d message %d \"%s\"",
              module_.c_str(), to.get_ip(), to.get_port(), n, buff_);
    client_.send_to(buff_, n, to, ec);
    EXPECT_FALSE(ec) << "module: " << module_ << ", " << ec.value() << " : "
                     << ec.message();
    ec.clear();
  }

  void complete(void *owner, const std::error_code &re_ec, size_t v) override {
    read(owner);
  }

private:
  SocketAddr to_;
  UdpSocket client_;
  char buff_[1024];
  std::string module_;

}; // class Udp

template <typename T> void ReactorUdpFunc() {
  std::error_code ec;
  SocketAddr addr(nullptr, "8989");
  LOG_TRACE("local ip is %s port %d", addr.get_ip(), addr.get_port());

  char port[8] = {};
  sprintf(port, "%d", addr.get_port());
  Udp<T> server("server");
  server.bind(port);
  EXPECT_TRUE(server.native_handle() > 0);

  T reactor(ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  reactor.post_read(server.native_handle(), &server, ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  LOG_TRACE("client begin connect server %s:%d", addr.get_ip(),
            addr.get_port());
  Udp<T> client("client");
  client.connect(addr);
  reactor.post_read(client.native_handle(), &client, ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  char buff[] = "client write message!";
  client.write(buff, sizeof(buff));

  LOG_TRACE("-------------------- begin run while --------------------");
  for (size_t i = 0; i < 10; ++i) {
    std::error_code ec;
    QueueOp queue;
    size_t size = reactor.run_once_timeout(queue, 1000, ec);
    EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
    ec.clear();
    LOG_TRACE("reactor timeout get size %d", size);
    reactor.call(queue);
  }
  LOG_TRACE("-------------------- end run while --------------------");

  client.close();
  server.close();

  reactor.close(ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
}

TEST(ReactorTest, ReactorUdp) { ReactorUdpFunc<Reactor>(); }

TEST(ReactorTest, SelectUdp) { ReactorUdpFunc<ReactorSelect>(); }
