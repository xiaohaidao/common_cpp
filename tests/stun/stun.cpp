
#include "gtest/gtest.h"

#include "sockets/TcpListener.h"
#include "sockets/TcpStream.h"
#include "stun/StunClient.h"
#include "utils/log.h"

/*
stun_server_tcp = [
    "fwa.lifesizecloud.com",
    "stun.isp.net.au",
    "stun.freeswitch.org",
    "stun.voip.blackberry.com",
    "stun.nextcloud.com",
    "stun.stunprotocol.org",
    "stun.sipnet.com",
    "stun.radiojar.com",
    "stun.sonetel.com",
    "stun.voipgate.com"
]
stun_server_udp = [
    "stun.miwifi.com",
    "stun.qq.com"
]
*/

TEST(StunTest, StunResponse) {
  char buff[] = {
      "\x01\x01\x00\x30\x21\x12\xa4\x42\x5a\xdc\x2f\xac\x27\x27\xdb\xbc"
      "\x59\xe8\x2e\x6d\x00\x01\x00\x08\x00\x01\x08\xd8\x71\x58\x0d\x84"
      "\x80\x2b\x00\x08\x00\x01\x0d\x96\x12\xbf\xdf\x0c\x80\x2c\x00\x08"
      "\x00\x01\x0d\x97\x12\xdb\x6e\x12\x00\x20\x00\x08\x00\x01\x29\xca"
      "\x50\x4a\xa9\xc6"}; // 113.88.13.132:2264

  size_t n = sizeof(buff) - 1;
  char out_buff[512] = {};
  for (size_t i = 0; i < n; ++i) {
    sprintf(out_buff + strlen(out_buff), "%x ", (uint8_t)buff[i]);
  }
  LOG_TRACE("response buff[%d]: %s", n, out_buff);
  SocketAddr addr = {};
  StunClient client;
  int res = client.response(buff, n, addr);
  EXPECT_TRUE(res >= 0) << "stun get response error! " << res;
  if (res >= 0) {
    LOG_TRACE("self buff ip port %s:%d", addr.get_ip(), addr.get_port());
  }
}

TEST(StunTest, StunClient) {
  StunClient client;
  char buff[1024] = {};
  int send_size = client.request(buff, sizeof(buff));
  EXPECT_TRUE(send_size > 0) << "stun get send buff error! " << send_size;

  std::error_code ec;
  LOG_TRACE("begin connect stun server");
  auto stun_addr =
      SocketAddr::resolve_host("stun.stunprotocol.org", "3478", ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  LOG_TRACE("connect stun server %s:%d", stun_addr.get_ip(),
            stun_addr.get_port());

  char port_buff[] = "41078";
  // auto stun_stream = TcpStream::connect(stun_addr, ec);
  auto stun_stream = TcpListener::bind_port(port_buff, ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  stun_stream.connected(stun_addr, ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  LOG_TRACE("stun client write buff");
  stun_stream.write(buff, send_size, ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  LOG_TRACE("stun client read");
  size_t n = stun_stream.read(buff, sizeof(buff), ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  char out_buff[1024] = {};
  for (size_t i = 0; i < n; ++i) {
    sprintf(out_buff + strlen(out_buff), "%x ", (uint8_t)buff[i]);
  }
  LOG_TRACE("response buff[%d]: %s", n, out_buff);
  SocketAddr addr = {};
  int res = client.response(buff, n, addr);
  EXPECT_TRUE(res >= 0) << "stun get response error! " << res;
  if (res >= 0) {
    LOG_TRACE("self buff ip port %s:%d", addr.get_ip(), addr.get_port());
  }

  // SocketAddr local_addr =
  // SocketAddr::get_local_socket(stun_stream.native_handle(), ec);
  // EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  // ec.clear();

  // LOG_TRACE("local ip port %s:%d", local_addr.get_ip(),
  // local_addr.get_port()); TcpListener listener = TcpListener::bind(port_buff,
  // ec); EXPECT_FALSE(ec) << ec.value() << " : " << ec.message(); ec.clear();

  // std::pair<TcpStream, SocketAddr> s = listener.accept(ec);
  // EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  // ec.clear();

  // char http_buff[] = {
  //   "GET /~ HTTP/1.1\r\n"
  //   "Connection: keep-alive\r\n"
  //   "\r\n"
  // };
  // s.first.write(http_buff, sizeof(http_buff),ec);
  // EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  // ec.clear();

  // s.first.close(ec);
  // EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  // ec.clear();
  // listener.close(ec);
  // EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  // ec.clear();

  stun_stream.close(ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
}
