
#include "gtest/gtest.h"

#include <map>

#include "reactor/Reactor.h"
#include "reactor/detail/QueueOp.h"
#include "sockets/SocketAddr.h"
#include "sockets/TcpListener.h"
#include "sockets/TcpStream.h"
#include "utils/log.h"

using namespace sockets;

template <typename T> class Tcp : public ReactorOp {
public:
  Tcp() = default;
  explicit Tcp(const char *module) : module_(module) {}

  Tcp(const TcpStream &stream, const char *module)
      : client_(stream), module_(module) {}

  void close() {
    LOG_TRACE("module: %s, close socket %d", module_.c_str(), native());
    std::error_code ec;
    client_.close(ec);
    EXPECT_FALSE(ec) << "module: " << module_ << ", " << ec.value() << " : "
                     << ec.message();
    ec.clear();
  }

  socket_type native() const { return client_.native(); }

  void connect(const SocketAddr &addr) {
    std::error_code ec;
    client_ = TcpStream::connect(addr, ec);
    EXPECT_FALSE(ec) << "module: " << module_ << ", " << ec.value() << " : "
                     << ec.message();
    ec.clear();
  }

  void write(const char *buff, size_t buff_size) {
    LOG_TRACE("module: %s, write message %d, \"%s\"", module_.c_str(),
              buff_size, buff);
    std::error_code ec;
    client_.write(buff, buff_size, ec);
    EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
    ec.clear();
  }

  void read(void *reactor) {
    LOG_TRACE("module: %s, socket %d begin read", module_.c_str(), native());
    std::error_code ec;
    size_t const n = client_.read(buff_, sizeof(buff_), ec);
    EXPECT_FALSE(ec) << "module: " << module_ << ", " << ec.value() << " : "
                     << ec.message();
    ec.clear();
    buff_[n] = 0;
    LOG_TRACE("module: %s, client read buff %d \"%s\"", module_.c_str(), n,
              buff_);

    if (n == 0) {
      LOG_TRACE("module: %s, close socket %d", module_.c_str(),
                client_.native());
      client_.close(ec);
      EXPECT_FALSE(ec) << "module: " << module_ << ", " << ec.value() << " : "
                       << ec.message();
      ec.clear();

      static_cast<T *>(reactor)->cancel(client_.native(), ec);
      EXPECT_FALSE(ec) << "module: " << module_ << ", " << ec.value() << " : "
                       << ec.message();
      ec.clear();
      return;
    }

    LOG_TRACE("module: %s, client write message %d \"%s\"", module_.c_str(), n,
              buff_);
    client_.write(buff_, n, ec);
    EXPECT_FALSE(ec) << "module: " << module_ << ", " << ec.value() << " : "
                     << ec.message();
    ec.clear();
  }

  void complete(void *owner, const std::error_code & /*re_ec*/,
                size_t /*v*/) override {
    read(owner);
  }

private:
  TcpStream client_;
  char buff_[1024];
  std::string module_;

}; // class Server

template <typename T> class Server : public ReactorOp {
public:
  explicit Server(const char *port, std::error_code &ec) {
    server_ = TcpListener::bind(port, ec);
  }

  void close() {
    for (auto &i : tcps_) {
      i.second.close();
    }
    tcps_.clear();
    LOG_TRACE("server close socket %d", native());
    std::error_code ec;
    server_.close(ec);
    EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
    ec.clear();
  }

  socket_type native() const { return server_.native(); }

  void accept(void *reactor) {
    LOG_TRACE("server socket %d begin accpet", native());
    std::error_code ec;
    std::pair<TcpStream, SocketAddr> const rec = server_.accept(ec);
    EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
    ec.clear();
    LOG_TRACE("server accpet client socket %d ip and port %s:%d",
              rec.first.native(), rec.second.get_ip(), rec.second.get_port());

    TcpStream const &st = rec.first;
    socket_type const s = st.native();
    tcps_[s] = Tcp<T>(st, "server");
    char buff[128] = "server begin send message";
    tcps_[s].write(buff, strlen(buff));

    static_cast<T *>(reactor)->post_read(s, &tcps_[s], ec);
    EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
    ec.clear();

    static_cast<T *>(reactor)->cancel(server_.native(), ec);
    EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
    ec.clear();
  }

  void complete(void *owner, const std::error_code & /*re_ec*/,
                size_t /*v*/) override {
    accept(owner);
  }

private:
  TcpListener server_;
  std::map<socket_type, Tcp<T> > tcps_;

}; // class Server

template <typename T> void reactor_func() {
  std::error_code ec;
  SocketAddr const addr(nullptr, "8988");
  LOG_TRACE("local ip is %s port %d", addr.get_ip(), addr.get_port());

  char port[8] = {};
  snprintf(port, sizeof(port), "%d", addr.get_port());
  Server<T> server(port, ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  EXPECT_TRUE(server.native() > 0);

  T reactor(ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  reactor.post_read(server.native(), &server, ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  LOG_TRACE("client begin connect server %s:%d", addr.get_ip(),
            addr.get_port());
  Tcp<T> client("client");
  client.connect(addr);
  reactor.post_read(client.native(), &client, ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  LOG_TRACE("-------------------- begin run while --------------------");
  for (size_t i = 0; i < 10; ++i) {
    std::error_code ec;
    QueueOp queue;
    size_t const size = reactor.run_once_timeout(queue, 1000, ec);
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

TEST(ReactorTest, ReactorTcp) { reactor_func<Reactor>(); }

TEST(ReactorTest, SelectTcp) { reactor_func<ReactorSelect>(); }
