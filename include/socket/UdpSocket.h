// Copyright (C) 2022 All rights reserved.
// Email: oxox0@qq.com. Created in 202207

namespace socket {

class UdpSocket {
  UdpSocket(const SocketAddr &addr) { int fd = socket(AF_INET, SOCK_DGRAM, 0); }

public:
  static std::unique_ptr<UdpSocket> connect(const SocketAddr &addr) {}

  void set_read_timeout(size_t timeout) {}

  void set_write_timeout(size_t timeout) {}

  size_t read_timeout() const {}

  size_t write_timeout() const {}

  // return receive size
  template <size_t N>
  std::pair<size_t, SocketAddr> recv_from(uint8_t &buf[N]) {}

  // return receive size
  std::pair<size_t, SocketAddr> recv_from(std::vector<uint8_t> &buf) {}

  // return send size
  template <size_t N>
  size_t send_to(const uint8_t &buf[N], const SocketAddr &addr) {}

  // return send size
  size_t send_to(const std::vector<uint8_t> &buf) {}
}; // class UdpSocket

} // namespace socket
