
#include "gtest/gtest.h"

#include "reactor/Select.h"
#include "sockets/SocketAddr.h"
#include "sockets/TcpListener.h"
#include "sockets/TcpStream.h"
#include "utils/log.h"

TEST(SocketsTest, SelectTcpBlock) {
  return;
  // server
  using namespace sockets;
  std::error_code ec;

  SocketAddr addr("127.0.0.1", "8981");
  LOG_TRACE("local ip is %s port %d", addr.get_ip(), addr.get_port());

  auto tcp = TcpListener::bind("8980", ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  EXPECT_TRUE(tcp.native_handle() > 0);
  LOG_TRACE("server socket value %d", tcp.native_handle());

  auto rec = tcp.accept(ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  tcp.close(ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  TcpStream &st = rec.first;
  char buff[128] = "server begin send message";
  st.write(buff, strlen(buff), ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  LOG_TRACE("client ip and port %s:%d", rec.second.get_ip(),
            rec.second.get_port());

  memset(buff, 0, sizeof(buff));
  size_t s = st.read(buff, sizeof(buff), ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  LOG_TRACE("read buff %s", buff);

  st.write(buff, strlen(buff), ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  st.close(ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  // client
  st = TcpStream::connect(addr, ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  EXPECT_TRUE(st.native_handle() > 0);
  LOG_TRACE("clinet socket value %d", st.native_handle());
  LOG_TRACE("client ip and port %s:%d", addr.get_ip(), addr.get_port());

  memset(buff, 0, sizeof(buff));
  s = st.read(buff, sizeof(buff), ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  LOG_TRACE("read buff %s", buff);
  if (s == 0) {
    st.close(ec);
    EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
    ec.clear();
  }

  st.write(buff, strlen(buff), ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  st.close(ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
}

TEST(SocketsTest, SelectTcp) {
  using namespace sockets;
  std::error_code ec;
  SocketAddr addr(nullptr, "8980");
  LOG_TRACE("local ip is %s port %d", addr.get_ip(), addr.get_port());

  char port[8] = {};
  sprintf(port, "%d", addr.get_port());
  auto tcp = TcpListener::bind(port, ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  EXPECT_TRUE(tcp.native_handle() > 0);

  Select s;
  s.post_read(tcp.native_handle(), [tcp](Select &sl) mutable {
    std::error_code ec;
    auto rec = tcp.accept(ec);
    EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
    ec.clear();

    tcp.close(ec);
    EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
    ec.clear();
    sl.depost(tcp.native_handle());

    TcpStream &st = rec.first;
    LOG_TRACE("client ip and port %s:%d", rec.second.get_ip(),
              rec.second.get_port());

    char buff[128] = "server begin send message";
    LOG_TRACE("server write message %d \"%s\"", strlen(buff), buff);
    st.write(buff, strlen(buff), ec);
    EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
    ec.clear();

    sl.post_read(st.native_handle(), [st](Select &sl) mutable {
      std::error_code ec;
      char buff[128] = {};
      size_t s = st.read(buff, sizeof(buff), ec);
      EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
      ec.clear();
      LOG_TRACE("server read buff %d \"%s\"", s, buff);

      LOG_TRACE("server write buff %d \"%s\"", s, buff);
      st.write(buff, s, ec);
      EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
      ec.clear();
    });
  });

  auto st = TcpStream::connect(addr, ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  EXPECT_TRUE(st.native_handle() > 0);

  s.post_read(st.native_handle(), [st](Select &sl) mutable {
    std::error_code ec;
    char buff[128] = {};
    size_t s = st.read(buff, sizeof(buff), ec);
    EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
    ec.clear();
    LOG_TRACE("client read buff %d \"%s\"", s, buff);
    if (s == 0) {
      st.close(ec);
      EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
      ec.clear();
      sl.depost(st.native_handle());
      return;
    }

    LOG_TRACE("client write message %d \"%s\"", s, buff);
    st.write(buff, s, ec);
    EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
    ec.clear();
  });

  LOG_TRACE("-------------------- begin run while --------------------");
  for (size_t i = 0; i < 10; ++i) {
    std::error_code ec;
    s.run_one(ec);
    EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
    ec.clear();

    s.run_one_timeout(1000, ec);
    EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
    ec.clear();
  }
  LOG_TRACE("-------------------- end run while --------------------");
}

// TEST(SocketsTest, SelectTcpAsync) {
//   using namespace sockets;
//   std::error_code ec;
//   SocketAddr addr(nullptr, "8980");
//   LOG_TRACE("local ip is %s port %d", addr.get_ip(), addr.get_port());

//   Select s;
//   auto st = TcpStream::async_connect(addr, ec);
//   EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
//   ec.clear();
//   EXPECT_TRUE(st.native_handle() > 0);

//   s.post_except(st.native_handle(), [st](Select &sl) mutable {
//     // never enter
//     LOG_TRACE("connect server error");

//     std::error_code ec;
//     st.close(ec);
//     EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
//     ec.clear();
//     sl.depost(st.native_handle());
//   });
//   s.post_write(st.native_handle(), [st](Select &sl) mutable {
//     LOG_TRACE("connect server success");

//     std::error_code ec;
//     sl.depost(st.native_handle());

//     sl.post_read(st.native_handle(), [st](Select &sl) mutable {
//       std::error_code ec;
//       char buff[128] = {};
//       size_t s = st.read(buff, sizeof(buff), ec);
//       EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
//       ec.clear();
//       LOG_TRACE("read buff \"%s\"", buff);
//       if (s == 0) {
//         st.close(ec);
//         EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
//         ec.clear();
//         sl.depost(st.native_handle());
//         return;
//       }

//       st.write(buff, strlen(buff), ec);
//       EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
//       ec.clear();
//     });
//   });
//   for (size_t i = 0; i < 3; ++i) {
//     std::error_code ec;
//     s.run_one(ec);
//     EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
//     ec.clear();

//     s.run_one_timeout(1000, ec);
//     EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
//     ec.clear();
//   }
// }
