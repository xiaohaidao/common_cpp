
#include "gtest/gtest.h"

#include "sockets/SocketAddr.h"
#include "sockets/UdpSocket.h"
#include "utils/log.h"

TEST(SocketsTest, UdpSocketTest) {
  std::error_code ec;
  // #ifdef WIN32
  // // set_multicast_interface err when not bind port
  UdpSocket server = UdpSocket::bind("9985", ec);
  // #else
  //   UdpSocket server = UdpSocket::create(kIpV4, ec);
  // #endif
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  LOG_DEBUG("server udp default value: loop %d ttl %d ",
            server.multicast_loop(ec), server.multicast_ttl(ec));
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  server.set_multicast_loop(true, ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  // braodcast
  LOG_DEBUG("server braodcast enable %d", server.broadcast(ec));
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  LOG_DEBUG("set server braodcast enable 1");
  server.set_broadcast(true, ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  LOG_DEBUG("server braodcast enable %d", server.broadcast(ec));
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  // connect
  server.connected({"www.baidu.com", "80"}, ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  SocketAddr local_addr = SocketAddr::get_local_socket(server.native(), ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  LOG_DEBUG("server socket value %d  %s:%d", server.native(),
            local_addr.get_ip(), local_addr.get_port());

  const char port[] = "8981";
  SocketAddr const addr("224.1.1.5", port);
  LOG_DEBUG("multicast ip is %s port %d", addr.get_ip(), addr.get_port());

  // initialize client
  UdpSocket client = UdpSocket::bind(port, ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  local_addr = SocketAddr::get_local_socket(client.native(), ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  LOG_DEBUG("clinet socket value %d  %s:%d", client.native(),
            local_addr.get_ip(), local_addr.get_port());

  // set loop
  client.set_multicast_loop(true, ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  client.set_multicast_ttl(10, ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  // get all local ip
  auto locals_ip = SocketAddr::get_local_ip_mask(ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  // joint multicast
  for (auto &i : locals_ip) {
    client.joint_multicast(addr, std::get<0>(i), ec);
    LOG_WARN("joint %s multicast %s", std::get<0>(i).get_ip(),
             ec ? ec.message().c_str() : "success");
    ec.clear();
  }
  LOG_DEBUG("client udp value: loop %d ttl %d ",
            (uint8_t)client.multicast_loop(ec), client.multicast_ttl(ec));
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  char buff[] = "multicast message!";
  // send multicast message
  for (auto &i : locals_ip) {
    server.set_multicast_interface(std::get<0>(i), ec);
    EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
    ec.clear();
    LOG_DEBUG("send message to multicast %s !", std::get<0>(i).get_ip());
    server.send_to(buff, sizeof(buff), addr, ec);
#ifdef _WIN32
    if (ec) {
      LOG_WARN("send message to %s error", std::get<0>(i).get_ip());
    }
#else
    EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
#endif
    ec.clear();
  }
  memset(buff, 0, sizeof(buff));

  client.set_read_timeout(100, ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  // recv multicast message
  for (size_t i = 0; i < locals_ip.size(); ++i) {
    std::pair<size_t, SocketAddr> const rev =
        client.recv_from(buff, sizeof(buff), ec);
    if (!ec) {
      LOG_DEBUG("client recv from %s:%d message size %d : %s",
                rev.second.get_ip(), rev.second.get_port(), rev.first, buff);

    } else {
      LOG_WARN("client recv multicast msg error");
    }
    ec.clear();
  }

  // leave multicast
  for (auto &i : locals_ip) {
    client.leave_multicast(addr, std::get<0>(i), ec);
    LOG_WARN("leave %s multicast %s", std::get<0>(i).get_ip(),
             ec ? ec.message().c_str() : "success");
    ec.clear();
  }
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  server.close(ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  client.close(ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
}
