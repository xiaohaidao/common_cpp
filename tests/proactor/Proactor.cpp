
#include "gtest/gtest.h"

#include <array>

#include "proactor/Proactor.h"
#include "proactor/operation/TcpListenerOp.h"
#include "utils/log.h"

using namespace std::placeholders; // for _1, _2, _3...

class tcp {
public:
  explicit tcp(proactor &p, const char *module)
      : tcp_op_(&p), module_(module) {}
  explicit tcp(const tcp_stream_op &p, const char *module)
      : tcp_op_(p), module_(module) {}
  ~tcp() = default;

  socket_type native() const { return tcp_op_.native(); }

  void close() {
    LOG_DEBUG("module_: %s, close socket %d", module_.c_str(), native());
    std::error_code ec;
    tcp_op_.close(ec);
    EXPECT_FALSE(ec) << "module_: " << module_ << ", " << ec.value() << " : "
                     << ec.message();
  }

  void read(const std::error_code &re_ec, size_t size) {
    EXPECT_FALSE(re_ec) << "module_: " << module_ << ", " << re_ec.value()
                        << " : " << re_ec.message();
    LOG_DEBUG("%s: %d async read size %d %d \"%s\"", module_.c_str(), native(),
              size, strlen(buff_), buff_);
    async_write(buff_, size);
  }

  void write(const std::error_code &re_ec, size_t size) {
    EXPECT_FALSE(re_ec) << "module_: " << module_ << ", " << re_ec.value()
                        << " : " << re_ec.message();
    LOG_DEBUG("%s: %d async send buff_ complete %d %d \"%s\"", module_.c_str(),
              native(), size, strlen(buff_), buff_);
    async_read();
  }

  template <typename F>
  void connected(F f, const std::error_code &re_ec, size_t size) {
    EXPECT_FALSE(re_ec) << "module_: " << module_ << ", " << re_ec.value()
                        << " : " << re_ec.message();
    LOG_DEBUG("%s: async connect complete %d", module_.c_str(), size);
    f();
  }

  template <typename F> void async_connect(const socket_addr &addr, F f) {
    std::error_code ec;
    tcp_op_.async_connect(addr, std::bind(&tcp::connected<F>, this, f, _1, _2),
                          ec);
    EXPECT_FALSE(ec) << "module_: " << module_ << ", " << ec.value() << " : "
                     << ec.message();
  }

  void async_read() {
    memset(buff_, 0, sizeof(buff_));
    std::error_code ec;
    tcp_op_.async_read((char *)buff_, sizeof(buff_),
                       std::bind(&tcp::read, this, _1, _2), ec);
    EXPECT_FALSE(ec) << "module_: " << module_ << ", " << ec.value() << " : "
                     << ec.message();
  }

  void async_write(const char *b, size_t size) {
    size = (std::min)(size, sizeof(buff_));
    memcpy(&buff_, b, size);
    buff_[size] = 0;
    LOG_DEBUG("%s: write message \"%s\"", module_.c_str(), buff_);
    std::error_code ec;
    tcp_op_.async_write((char *)buff_, size,
                        std::bind(&tcp::write, this, _1, _2), ec);
    EXPECT_FALSE(ec) << "module_: " << module_ << ", " << ec.value() << " : "
                     << ec.message();
  }

private:
  char buff_[1024];
  tcp_stream_op tcp_op_;
  std::string module_;
};

template <size_t listeners_size = 0> class service {
public:
  service(proactor &p) : listener_(p) {}
  ~service() = default;

  socket_type native() const { return listener_.native(); }

  void close() {
    for (auto &i : tcps_) {
      i.close();
    }
    LOG_DEBUG("server close socket %d", native());
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
              const std::pair<tcp_stream_op, socket_addr> &ac) {

    LOG_DEBUG("server socket %d begin accpet", native());

    EXPECT_FALSE(re_ec) << re_ec.value() << " : " << re_ec.message();
    tcp service_tcp(ac.first, "server");
    LOG_DEBUG("socket %d client ip and port %s:%d", service_tcp.native(),
              ac.second.get_ip(), ac.second.get_port());

    tcps_.push_back(std::move(service_tcp));
    tcp &alias = *tcps_.rbegin();
    alias.async_write("Server send message", 19);
    async_accept();
  }

  void async_accept(size_t index) {
    if (listeners_.size() >= index) {
      return;
    }
    std::error_code ec;
    listeners_[index].async_accept(std::bind(&service::accept, this, _1, _2),
                                   ec);
    EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
    ec.clear();
  }

  void async_accept() {
    std::error_code ec;
    listener_.async_accept(std::bind(&service::accept, this, _1, _2), ec);
    EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
    ec.clear();
  }

private:
  tcp_listener_op listener_;
  std::array<tcp_listener_op, listeners_size> listeners_;
  std::vector<tcp> tcps_;
};

TEST(ProactorTest, Proactor) {
  std::error_code ec;
  proactor p(ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  socket_addr const addr(nullptr, "8989");
  LOG_DEBUG("local ip is %s port %d", addr.get_ip(), addr.get_port());
  char port[8] = {};
  snprintf(port, sizeof(port), "%d", addr.get_port());

  service<> server(p);

  LOG_DEBUG("bind port %s", port);
  server.bind(port);

  LOG_DEBUG("server accept");
  server.async_accept();

  tcp client(p, "client");

  LOG_DEBUG("client async connect");
  auto client_call_back = [&client]() {
    LOG_DEBUG("client async read");
    client.async_read();
    client.async_write("client write", 13);
  };
  client.async_connect(addr, client_call_back);

  LOG_DEBUG("-------------------- begin run while --------------------");
  for (size_t i = 0; i < 10; ++i) {
    p.run_one(1000ull * 1000ull, ec);
    EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
    ec.clear();
  }
  LOG_DEBUG("-------------------- end run while --------------------");
  client.close();
  server.close();

  p.close(ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
}
