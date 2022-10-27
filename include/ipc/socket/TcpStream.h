// Copyright (C) 2022 All rights reserved.
// Email: oxox0@qq.com. Created in 202207

namespace socket {

class TcpStream {
  TcpStream() { int fd = socket(AF_INET, SOCK_STREAM, 0); }

public:
  static std::unique_ptr<TcpStream> connect(const SocketAddr &addr) {}

  static std::unique_ptr<TcpStream> connect_timeout(const SocketAddr &addr,
                                                    size_t time_out) {}

  bool shutdown() {}

  void set_read_timeout(size_t timeout) {}

  void set_write_timeout(size_t timeout) {}

  size_t read_timeout() const {}

  size_t write_timeout() const {}

  // return read size
  template <size_t N> size_t read(uint8_t &buf[N]) {}

  // return read size
  size_t read(std::vector<uint8_t> &buf) {}

  // return write size
  template <size_t N> size_t write(const uint8_t &buf[N]) {}

  // return write size
  size_t write(const std::vector<uint8_t> &buf) {}

}; // class TcpStream

} // namespace socket
