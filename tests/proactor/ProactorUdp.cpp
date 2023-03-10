
#include "gtest/gtest.h"

#include <array>

#include "proactor/Proactor.h"
#include "proactor/operation/UdpSocketOp.h"
#include "utils/log.h"

using namespace std::placeholders; // for _1, _2, _3...

class Udp {
public:
  Udp(Proactor &p, const char *module)
      : buff_{}, udp_op_(&p), module_(module) {}
  ~Udp() {}

  socket_type native_handle() const { return udp_op_.native_handle(); }

  void bind(char *port_service) {
    std::error_code ec;
    udp_op_.bind(port_service, ec);
    EXPECT_FALSE(ec) << "module: " << module_ << ", " << ec.value() << " : "
                     << ec.message();
  }

  void create() {
    std::error_code ec;
    udp_op_.create(kIpV4, ec);
    EXPECT_FALSE(ec) << "module: " << module_ << ", " << ec.value() << " : "
                     << ec.message();
  }

  void close() {
    LOG_TRACE("module: %s, close socket %d", module_.c_str(), native_handle());
    std::error_code ec;
    udp_op_.close(ec);
    EXPECT_FALSE(ec) << "module: " << module_ << ", " << ec.value() << " : "
                     << ec.message();
  }

  void read(const std::error_code &re_ec, size_t size, const SocketAddr &from) {
    EXPECT_FALSE(re_ec) << "module: " << module_ << ", " << re_ec.value()
                        << " : " << re_ec.message();
    LOG_TRACE("%s: %d async read from %s:%d size %d %d \"%s\"", module_.c_str(),
              native_handle(), from.get_ip(), from.get_port(), size,
              strlen(buff_), buff_);
    if (!re_ec)
      async_write(buff_, size, from);
  }

  void write(const std::error_code &re_ec, size_t size) {
    EXPECT_FALSE(re_ec) << "module: " << module_ << ", " << re_ec.value()
                        << " : " << re_ec.message();
    LOG_TRACE("%s: %d async send buff complete %d %d \"%s\"", module_.c_str(),
              native_handle(), size, strlen(buff_), buff_);
    if (!re_ec)
      async_read();
  }

  void async_read() {
    memset(buff_, 0, sizeof(buff_));
    std::error_code ec;
    udp_op_.async_read((char *)buff_, sizeof(buff_),
                       std::bind(&Udp::read, this, _1, _2, _3), ec);
    EXPECT_FALSE(ec) << "module: " << module_ << ", " << ec.value() << " : "
                     << ec.message();
  }

  void async_write(const char *buff, size_t size, const SocketAddr &to) {
    size = (std::min)(size, sizeof(buff_));
    memcpy(buff_, buff, size);
    buff_[size] = 0;
    LOG_TRACE("%s: write to %s:%d message \"%s\"", module_.c_str(), to.get_ip(),
              to.get_port(), buff_);
    std::error_code ec;
    udp_op_.async_write((char *)buff_, size, to,
                        std::bind(&Udp::write, this, _1, _2), ec);
    EXPECT_FALSE(ec) << "module: " << module_ << ", " << ec.value() << " : "
                     << ec.message();
  }

  char buff_[1024];
  UdpSocketOp udp_op_;
  std::string module_;
};

TEST(ProactorTest, ProactorUdp) {
  std::error_code ec;
  Proactor p(ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  SocketAddr addr(nullptr, "8989");
  LOG_TRACE("local ip is %s port %d", addr.get_ip(), addr.get_port());
  char port[8] = {};
  sprintf(port, "%d", addr.get_port());

  Udp server(p, "Server");

  LOG_TRACE("bind port %s", port);
  server.bind(port);
  server.async_read();

  Udp client(p, "client");
  client.create();

  char buff[] = "client udp send message!";
  client.async_write(buff, sizeof(buff), addr);

  LOG_TRACE("-------------------- begin run while --------------------");
  for (size_t i = 0; i < 10; ++i) {
    p.run_one(1000 * 1000, ec);
    EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
    ec.clear();
  }
  LOG_TRACE("-------------------- end run while --------------------");
  client.close();
  server.close();

  p.close(ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
}
