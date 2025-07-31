
#include "gtest/gtest.h"

#include <array>

#include "proactor/Proactor.h"
#include "proactor/operation/UdpSocketOp.h"
#include "utils/log.h"

using namespace std::placeholders; // for _1, _2, _3...

class udp {
public:
  udp(proactor &p, const char *module) : buff_{}, udp_op_(p), module_(module) {}
  ~udp() = default;

  socket_type native() const { return udp_op_.native(); }

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
    LOG_DEBUG("module: %s, close socket %d", module_.c_str(), native());
    std::error_code ec;
    udp_op_.close(ec);
    EXPECT_FALSE(ec) << "module: " << module_ << ", " << ec.value() << " : "
                     << ec.message();
  }

  void read(const std::error_code &re_ec, size_t size,
            const socket_addr &from) {
    EXPECT_FALSE(re_ec) << "module: " << module_ << ", " << re_ec.value()
                        << " : " << re_ec.message();
    LOG_DEBUG("%s: %d async read from %s:%d size %d %d \"%s\"", module_.c_str(),
              native(), from.get_ip(), from.get_port(), size, strlen(buff_),
              buff_);
    if (!re_ec)
      async_write(buff_, size, from);
  }

  void write(const std::error_code &re_ec, size_t size) {
    EXPECT_FALSE(re_ec) << "module: " << module_ << ", " << re_ec.value()
                        << " : " << re_ec.message();
    LOG_DEBUG("%s: %d async send buff complete %d %d \"%s\"", module_.c_str(),
              native(), size, strlen(buff_), buff_);
    if (!re_ec)
      async_read();
  }

  void async_read() {
    memset(buff_, 0, sizeof(buff_));
    std::error_code ec;
    udp_op_.async_read((char *)buff_, sizeof(buff_),
                       std::bind(&udp::read, this, _1, _2, _3), ec);
    EXPECT_FALSE(ec) << "module: " << module_ << ", " << ec.value() << " : "
                     << ec.message();
  }

  void async_write(const char *buff, size_t size, const socket_addr &to) {
    size = (std::min)(size, sizeof(buff));
    memcpy(buff_, buff, size);
    buff_[size] = 0;
    LOG_DEBUG("%s: write to %s:%d message \"%s\"", module_.c_str(), to.get_ip(),
              to.get_port(), buff_);
    std::error_code ec;
    udp_op_.async_write((char *)buff_, size, to,
                        std::bind(&udp::write, this, _1, _2), ec);
    EXPECT_FALSE(ec) << "module: " << module_ << ", " << ec.value() << " : "
                     << ec.message();
  }

private:
  char buff_[1024];
  udp_socket_op udp_op_;
  std::string module_;
};

TEST(ProactorTest, ProactorUdp) {
  std::error_code ec;
  proactor p(ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  socket_addr const addr(nullptr, "8989");
  LOG_DEBUG("local ip is %s port %d", addr.get_ip(), addr.get_port());
  char port[8] = {};
  snprintf(port, sizeof(port), "%d", addr.get_port());

  udp server(p, "Server");

  LOG_DEBUG("bind port %s", port);
  server.bind(port);
  server.async_read();

  udp client(p, "client");
  client.create();

  char buff[] = "client udp send message!";
  client.async_write(buff, sizeof(buff), addr);
  client.async_read();

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
