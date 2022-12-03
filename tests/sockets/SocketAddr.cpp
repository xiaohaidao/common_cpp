
#include "gtest/gtest.h"

#include "sockets/SocketAddr.h"
#include "utils/log.h"

TEST(SocketsTest, SocketAddr) {
  using namespace sockets;
  std::error_code ec;
  auto local = SocketAddr::resolve_host(nullptr, "http", ec, kIpV6);
  // auto local = SocketAddr::resolve_host(nullptr, "80", ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  LOG_TRACE("local ip is %s port %d", local.get_ip(), local.get_port());

  char buff1[64] = {}, buff2[64] = {};
  local.get_nameinfo(buff1, sizeof(buff1), buff2, sizeof(buff2), ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  LOG_TRACE("hostname %s service %s", buff1, buff2);

  const char *host = SocketAddr::get_localhost(ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  LOG_TRACE("localhost name %s", host);

  local = SocketAddr::resolve_host("www.baidu.com", nullptr, ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  LOG_TRACE("www.baidu.com ip is %s port %d", local.get_ip(), local.get_port());
  buff1[64] = {}, buff2[64] = {};
  local.get_nameinfo(buff1, sizeof(buff1), buff2, sizeof(buff2), ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  LOG_TRACE("hostname %s service %s", buff1, buff2);

  auto local_all = SocketAddr::resolve_host_all("www.jd.com", "ftp", ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  for (auto &i : local_all) {
    LOG_TRACE("www.jd.com ip is %s port %d", i.get_ip(), i.get_port());
  }

  local = SocketAddr::resolve_host("192.168.0.1", "http", ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  LOG_TRACE("local ip is %s port %d", local.get_ip(), local.get_port());
  buff1[64] = {}, buff2[64] = {};
  local.get_nameinfo(buff1, sizeof(buff1), buff2, sizeof(buff2), ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  LOG_TRACE("hostname %s service %s", buff1, buff2);
}
