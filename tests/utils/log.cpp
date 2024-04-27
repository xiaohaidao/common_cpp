
#include "gtest/gtest.h"

#include "sockets/UdpSocket.h"
#include "utils/log.h"

void callback(LogLevel, const char *data, int size) {
  static bool init = false;
  static bool use_udp = false;
  std::error_code ec;
#ifdef _WIN32
  static SocketAddr const kLogAddr(nullptr, "514");
  static UdpSocket udp = UdpSocket::create(kIpV4, ec);
#else
  static SocketAddr const kLogAddr("/dev/log");
  static UdpSocket udp = UdpSocket::create(kUnix, ec);
#endif
  if (!init) {
    udp.connected(kLogAddr, ec);
    if (!ec) {
      init = true;
      use_udp = true;
    } else {
      udp.close(ec);
    }
  }
  if (use_udp) {
    udp.send_to(data, (size_t)size, kLogAddr, ec);
  } else {
    printf(data);
  }
}

TEST(LogTest, log) {
  set_export_callback(callback);
  LOG_DEBUG("log debug int %d, double %f, str %s .", 1, 1.0, "1");
  LOG_INFO("log info int %d, double %f, str %s .", 1, 1.0, "1");
  LOG_WARN("log warn int %d, double %f, str %s .", 1, 1.0, "1");
  LOG_ERROR("log error int %d, double %f, str %s .", 1, 1.0, "1");

  LOG_DEBUG("log debug int %d, double %f, str %s .\n", 1, 1.0, "1");
  LOG_INFO("log info int %d, double %f, str %s .\n", 1, 1.0, "1");
  LOG_WARN("log warn int %d, double %f, str %s .\n", 1, 1.0, "1");
  LOG_ERROR("log error int %d, double %f, str %s .\n", 1, 1.0, "1");
  set_export_callback(nullptr);

  constexpr char kBuff1024Char[] =
      "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
      "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
      "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
      "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
      "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
      "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
      "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
      "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
      "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
      "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
      "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
      "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
      "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
      "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
      "ffffffffffffffff";
  constexpr char kBuff915Char[] =
      "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
      "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
      "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
      "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
      "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
      "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
      "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
      "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
      "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
      "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
      "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
      "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
      "fffffffffffffffffffffffffffffffffffffffffffff";
  LOG_ERROR("log error int %d, double %f, str %s ."
            "%s"
            ".....",
            1, 1.0, "1", kBuff1024Char);
  LOG_ERROR("%s", kBuff915Char);

  set_skip_filename(1);
  LOG_DEBUG("log debug int %d, double %f, str %s .", 1, 1.0, "1");
  LOG_INFO("log info int %d, double %f, str %s .", 1, 1.0, "1");
  LOG_WARN("log warn int %d, double %f, str %s .", 1, 1.0, "1");
  LOG_ERROR("log error int %d, double %f, str %s .", 1, 1.0, "1");
  set_skip_funname(0);
  LOG_DEBUG("log debug int %d, double %f, str %s .", 1, 1.0, "1");
  LOG_INFO("log info int %d, double %f, str %s .", 1, 1.0, "1");
  LOG_WARN("log warn int %d, double %f, str %s .", 1, 1.0, "1");
  LOG_ERROR("log error int %d, double %f, str %s .", 1, 1.0, "1");
  set_skip_filename(0);
  LOG_DEBUG("log debug int %d, double %f, str %s .", 1, 1.0, "1");
  LOG_INFO("log info int %d, double %f, str %s .", 1, 1.0, "1");
  LOG_WARN("log warn int %d, double %f, str %s .", 1, 1.0, "1");
  LOG_ERROR("log error int %d, double %f, str %s .", 1, 1.0, "1");
}
