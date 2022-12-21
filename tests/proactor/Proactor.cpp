
#include "gtest/gtest.h"

#include <array>

#include "proactor/Proactor.h"
#include "proactor/operation/TcpListenerOp.h"
#include "utils/log.h"

using namespace std::placeholders; // for _1, _2, _3...

class Tcp {
public:
  explicit Tcp(Proactor &p, const char *module) : tcp_op(p), module_(module) {}
  explicit Tcp(TcpStreamOp &&p, const char *module)
      : tcp_op(std::move(p)), module_(module) {}
  ~Tcp() {}

  void close() {
    std::error_code ec;
    tcp_op.close(ec);
    EXPECT_FALSE(ec) << "module: " << module_ << ", " << ec.value() << " : "
                     << ec.message();
  }

  void read(const std::error_code &re_ec, size_t size) {
    EXPECT_FALSE(re_ec) << "module: " << module_ << ", " << re_ec.value()
                        << " : " << re_ec.message();
    LOG_TRACE("%s: async read size %d %d \"%s\"", module_.c_str(), size,
              strlen(buff_), buff_);
    async_write(buff_, size);
  }

  void write(const std::error_code &re_ec, size_t size) {
    EXPECT_FALSE(re_ec) << "module: " << module_ << ", " << re_ec.value()
                        << " : " << re_ec.message();
    LOG_TRACE("%s: async send buff complete %d %d \"%s\"", module_.c_str(),
              size, strlen(buff_), buff_);
    async_read();
  }

  template <typename F>
  void connected(F f, const std::error_code &re_ec, size_t size) {
    EXPECT_FALSE(re_ec) << "module: " << module_ << ", " << re_ec.value()
                        << " : " << re_ec.message();
    LOG_TRACE("%s: async connect complete %d", module_.c_str(), size);
    f();
  }

  template <typename F>
  void async_connect(const sockets::SocketAddr &addr, F f) {
    std::error_code ec;
    tcp_op.async_connect(addr, std::bind(&Tcp::connected<F>, this, f, _1, _2),
                         ec);
    EXPECT_FALSE(ec) << "module: " << module_ << ", " << ec.value() << " : "
                     << ec.message();
  }

  void async_read() {
    memset(buff_, 0, sizeof(buff_));
    std::error_code ec;
    tcp_op.async_read((char *)buff_, sizeof(buff_),
                      std::bind(&Tcp::read, this, _1, _2), ec);
    EXPECT_FALSE(ec) << "module: " << module_ << ", " << ec.value() << " : "
                     << ec.message();
  }

  void async_write(const char *buff, size_t size) {
    size = (std::min)(size, sizeof(buff_));
    memcpy(buff_, buff, size);
    LOG_TRACE("%s: write message \"%s\"", module_.c_str(), buff_);
    std::error_code ec;
    tcp_op.async_write((char *)buff_, size,
                       std::bind(&Tcp::write, this, _1, _2), ec);
    EXPECT_FALSE(ec) << "module: " << module_ << ", " << ec.value() << " : "
                     << ec.message();
  }

  char buff_[1024];
  TcpStreamOp tcp_op;
  std::string module_;
};

template <size_t listeners_size = 0> class Service {
public:
  Service(Proactor &p) : listener_(p) {}
  ~Service() {}

  void close() {
    for (auto &i : tcps_) {
      i.close();
    }
    std::error_code ec;
    listener_.close(ec);
    EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  }

  void bind(const char *port) {
    std::error_code ec;
    listener_.bind(port, ec);
    EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
    ec.clear();
    for (auto &i : listeners_) {
      i = listener_;
    }
  }

  void accept(const std::error_code &re_ec,
              std::pair<TcpStreamOp, sockets::SocketAddr> ac) {

    EXPECT_FALSE(re_ec) << re_ec.value() << " : " << re_ec.message();
    Tcp service_tcp(std::move(ac.first), "server");
    LOG_TRACE("socket %d client ip and port %s:%d",
              service_tcp.tcp_op.native_handle(), ac.second.get_ip(),
              ac.second.get_port());

    tcps_.push_back(std::move(service_tcp));
    Tcp &alias = *tcps_.rbegin();
    alias.async_write("Server send message", 19);
    async_accept();
  }

  void async_accept(size_t index) {
    if (listeners_.size() >= index) {
      return;
    }
    std::error_code ec;
    listeners_[index].async_accept(std::bind(&Service::accept, this, _1, _2),
                                   ec);
    EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
    ec.clear();
  }

  void async_accept() {
    std::error_code ec;
    listener_.async_accept(std::bind(&Service::accept, this, _1, _2), ec);
    EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
    ec.clear();
  }

  TcpListenerOp listener_;
  std::array<TcpListenerOp, listeners_size> listeners_;
  std::vector<Tcp> tcps_;
};

TEST(ProactorTest, Proactor) {
  using sockets::SocketAddr;
  std::error_code ec;
  Proactor p = Proactor::create(ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  SocketAddr addr(nullptr, "8980");
  LOG_TRACE("local ip is %s port %d", addr.get_ip(), addr.get_port());
  char port[8] = {};
  sprintf(port, "%d", addr.get_port());

  Service<> server(p);

  LOG_TRACE("bind port %s", port);
  server.bind(port);

  LOG_TRACE("server accept");
  server.async_accept();

  Tcp client(p, "client");

  LOG_TRACE("client async connect");
  auto client_call_back = [&client]() {
    LOG_TRACE("client async read");
    client.async_read();
  };
  client.async_connect(addr, client_call_back);

  LOG_TRACE("-------------------- begin run while --------------------");
  for (size_t i = 0; i < 10; ++i) {
    p.run_one(1000 * 1000, ec);
    EXPECT_FALSE(ec && ec.value() != WAIT_TIMEOUT)
        << ec.value() << " : " << ec.message();
    ec.clear();
  }
  LOG_TRACE("-------------------- end run while --------------------");
  server.close();
  client.close();
}
