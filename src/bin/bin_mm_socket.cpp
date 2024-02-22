
#include "ipc/mm_socket.h"
#include "sockets/TcpListener.h"
#include "utils/log.h"
#include "utils/random.h"

void local_unix() {
#ifdef __linux__
  SocketAddr a("local_socket");
  std::error_code ec;
  auto client = TcpStream::connect(a, ec);
  bool is_server = ec ? true : false;
  if (is_server) {
    ec.clear();
    TcpListener server = TcpListener::bind("local_socket", kUnix, ec);
    // EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
    ec.clear();

    auto s_c = server.accept(ec);
    bool ok = ec ? false : true;
    // EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
    ec.clear();

    LOG_TRACE("accept from %s:%d", s_c.second.get_ip(), s_c.second.get_port());

    auto &s_client = s_c.first;
    // read and write
    size_t send_size = 0, recv_size = 0;
    size_t i = 0, first_size = 0;
    for (i = 0; i < (size_t)(1024 * 1024 * 8) && ok; ++i) {
      // LOG_TRACE("read write index %d", i);
      char buff[256] = {};
      size_t ret = s_client.read(buff, sizeof(buff) - 1, ec);
      // LOG_TRACE("server receive msg size %d:%d", ret, strlen(buff) + 1);
      // EXPECT_TRUE(ret == 0 || ret == strlen(buff) + 1);
      // EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
      ec.clear();
      recv_size += ret;
      if (!first_size) {
        first_size = ret;
      }
      if (first_size != ret && ret > 0) {
        LOG_DEBUG("diff size %d %d", first_size, ret);
        break;
      }

      ret = s_client.write(buff, strlen(buff) + 1, ec);
      // LOG_TRACE("server send msg size %d:%d", ret, strlen(buff) + 1);
      // EXPECT_TRUE(ret == strlen(buff) + 1);
      // EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
      ec.clear();
      send_size += ret;
      if (first_size != ret && ret > 0) {
        LOG_DEBUG("diff size %d %d", first_size, ret);
        break;
      }
    }
    LOG_TRACE("count %d send size %d and recv size %d", i, send_size,
              recv_size);

    s_client.close(ec);
    // EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
    ec.clear();
    server.close(ec);
    // EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
    ec.clear();
    return;
  }
  bool ok = true;
  // read and write
  size_t send_size = 0, recv_size = 0;
  size_t i = 0, first_size = 0;
  for (i = 0; i < (size_t)(1024 * 1024 * 8) && ok; ++i) {
    // LOG_TRACE("read write index %d", i);
    char buff[256] = {};
    const char message[] = "client send message";
    char msg_buff[256] = {};
    snprintf(msg_buff, sizeof(msg_buff) - 1, "%s:%.100zu", message, i);
    size_t ret = client.write(msg_buff, strlen(msg_buff) + 1, ec);
    // EXPECT_TRUE(ret == strlen(msg_buff) + 1);
    // EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
    ec.clear();
    send_size += ret;
    if (!first_size) {
      first_size = ret;
    }
    if (first_size != ret && ret > 0) {
      LOG_DEBUG("diff size %d %d", first_size, ret);
      break;
    }

    ret = client.read(buff, sizeof(buff) - 1, ec);
    // LOG_TRACE("client receive msg size %d:%d", ret, strlen(buff) + 1);
    // EXPECT_TRUE(ret == 0 || ret == strlen(buff) + 1);
    // EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
    ec.clear();
    recv_size += ret;
    if (first_size != ret && ret > 0) {
      LOG_DEBUG("diff size %d %d", first_size, ret);
      break;
    }
  }
  LOG_TRACE("count %d send size %d and recv size %d", i, send_size, recv_size);

  client.close(ec);
  // EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
#endif // __linux__
}

void tcp() {
  SocketAddr const a("127.0.0.1", "8980");
  std::error_code ec;
  auto client = TcpStream::connect(a, ec);
  bool const is_server = ec ? true : false;
  if (is_server) {
    ec.clear();
    TcpListener server = TcpListener::bind("8980", ec);
    // EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
    ec.clear();

    auto s_c = server.accept(ec);
    bool const ok = ec ? false : true;
    // EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
    ec.clear();

    LOG_TRACE("accept from %s:%d", s_c.second.get_ip(), s_c.second.get_port());

    auto &s_client = s_c.first;
    // read and write
    size_t send_size = 0, recv_size = 0;
    size_t i = 0, first_size = 0;
    for (i = 0; i < (size_t)(1024 * 1024 * 8) && ok; ++i) {
      // LOG_TRACE("read write index %d", i);
      char buff[256] = {};
      size_t ret = s_client.read(buff, sizeof(buff) - 1, ec);
      // LOG_TRACE("server receive msg size %d:%d", ret, strlen(buff) + 1);
      // EXPECT_TRUE(ret == 0 || ret == strlen(buff) + 1);
      // EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
      ec.clear();
      recv_size += ret;
      if (!first_size) {
        first_size = ret;
      }
      if (first_size != ret && ret > 0) {
        LOG_DEBUG("diff size %d %d", first_size, ret);
        break;
      }

      ret = s_client.write(buff, strlen(buff) + 1, ec);
      // LOG_TRACE("server send msg size %d:%d", ret, strlen(buff) + 1);
      // EXPECT_TRUE(ret == strlen(buff) + 1);
      // EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
      ec.clear();
      send_size += ret;
      if (first_size != ret && ret > 0) {
        LOG_DEBUG("diff size %d %d", first_size, ret);
        break;
      }
    }
    LOG_TRACE("count %d send size %d and recv size %d", i, send_size,
              recv_size);

    s_client.close(ec);
    // EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
    ec.clear();
    server.close(ec);
    // EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
    ec.clear();
    return;
  }
  bool const ok = true;
  // read and write
  size_t send_size = 0, recv_size = 0;
  size_t i = 0, first_size = 0;
  for (i = 0; i < (size_t)(1024 * 1024 * 8) && ok; ++i) {
    // LOG_TRACE("read write index %d", i);
    char buff[256] = {};
    const char message[] = "client send message";
    char msg_buff[256] = {};
    snprintf(msg_buff, sizeof(msg_buff) - 1, "%s:%.100zu", message, i);
    size_t ret = client.write(msg_buff, strlen(msg_buff) + 1, ec);
    // EXPECT_TRUE(ret == strlen(msg_buff) + 1);
    // EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
    ec.clear();
    send_size += ret;
    if (!first_size) {
      first_size = ret;
    }
    if (first_size != ret && ret > 0) {
      LOG_DEBUG("diff size %d %d", first_size, ret);
      break;
    }

    ret = client.read(buff, sizeof(buff) - 1, ec);
    // LOG_TRACE("client receive msg size %d:%d", ret, strlen(buff) + 1);
    // EXPECT_TRUE(ret == 0 || ret == strlen(buff) + 1);
    // EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
    ec.clear();
    recv_size += ret;
    if (first_size != ret && ret > 0) {
      LOG_DEBUG("diff size %d %d", first_size, ret);
      break;
    }
  }
  LOG_TRACE("count %d send size %d and recv size %d", i, send_size, recv_size);

  client.close(ec);
  // EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
}

void c() {
  const char server[] = "bind_mm";

  mm_socket s;
  int ret = s.bind(server);
  LOG_TRACE("server bind %d", ret);
  bool const is_server = !(ret < 0);

  if (is_server) {
    mm_client s_client;

    // connect
    LOG_TRACE("connect");
    do {
      ret = s.accept(s_client);
      LOG_TRACE("server connect %d", ret);
      // EXPECT_TRUE(ret > 0);
    } while (ret != 0);

    bool const ok = s_client.send(nullptr, 0) == 0;
    LOG_ERROR("ok %d", ok);

    // read and write
    size_t send_size = 0, recv_size = 0;
    size_t i = 0, first_size = 0;
    for (i = 0; i < (size_t)(1024 * 1024 * 8) && ok; ++i) {
      // LOG_TRACE("read write index %d", i);
      char buff[256] = {};
      do {
        ret = s_client.recv(buff, sizeof(buff) - 1);
        // LOG_TRACE("server receive msg size %d:%d", ret, strlen(buff) + 1);
        // EXPECT_TRUE(ret == 0 || ret == strlen(buff) + 1);
        recv_size += ret;
        if (!first_size) {
          first_size = ret;
        }
        if (first_size != ret && ret > 0) {
          LOG_DEBUG("diff size %d %d", first_size, ret);
          break;
        }
      } while (ret == 0);

      ret = s_client.send(buff, strlen(buff) + 1);
      // LOG_TRACE("server send msg size %d:%d", ret, strlen(buff) + 1);
      // EXPECT_TRUE(ret == strlen(buff) + 1);
      send_size += ret;
      if (first_size != ret && ret > 0) {
        LOG_DEBUG("diff size %d %d", first_size, ret);
        break;
      }
    }
    LOG_TRACE("count %d send size %d and recv size %d", i, send_size,
              recv_size);

    // close
    s_client.close();
    s.close();
    LOG_TRACE("mm socket end");
    return;
  }
  mm_client client;

  // connect
  LOG_TRACE("connect");
  do {
    ret = client.connect(server);
    LOG_TRACE("client connect %d", ret);
    // EXPECT_TRUE(ret > 0);
  } while (ret != 0);

  bool const ok = client.send(nullptr, 0) == 0;
  LOG_ERROR("ok %d", ok);

  // read and write
  size_t send_size = 0, recv_size = 0;
  size_t i = 0, first_size = 0;
  for (i = 0; i < (size_t)(1024 * 1024 * 8) && ok; ++i) {
    // LOG_TRACE("read write index %d", i);
    char buff[256] = {};
    const char message[] = "client send message";
    char msg_buff[256] = {};
    snprintf(msg_buff, sizeof(msg_buff) - 1, "%s:%.100zu", message, i);
    ret = client.send(msg_buff, strlen(msg_buff) + 1);
    // LOG_TRACE("client send msg size %d:%d", ret, strlen(msg_buff) + 1);
    // EXPECT_TRUE(ret == strlen(msg_buff) + 1);
    send_size += ret;
    if (!first_size) {
      first_size = ret;
    }
    if (first_size != ret && ret > 0) {
      LOG_DEBUG("diff size %d %d", first_size, ret);
      break;
    }

    do {
      ret = client.recv(buff, sizeof(buff) - 1);
      // LOG_TRACE("client receive msg size %d:%d", ret, strlen(buff) + 1);
      // EXPECT_TRUE(ret == 0 || ret == strlen(buff) + 1);
      recv_size += ret;
      if (first_size != ret && ret > 0) {
        LOG_DEBUG("diff size %d %d", first_size, ret);
        break;
      }
    } while (ret == 0);
  }
  LOG_TRACE("count %d send size %d and recv size %d", i, send_size, recv_size);

  // close
  client.close();
  s.close();
  LOG_TRACE("mm socket end");
}

int main() {
  c();
  local_unix();
  tcp();
}