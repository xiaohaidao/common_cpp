// Copyright (C) 2022 All rights reserved.
// Email: oxox0@qq.com. Created in 202207

namespace socket {

class TcpListener {
  TcpListener(const SocketAddr &addr) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
  }

public:
  static std::unique_ptr<TcpListener> bind(const SocketAddr &addr) {}

  std::pair<std::unique_ptr<TcpStream>, SocketAddr> accept() {}

}; // class TcpListener

} // namespace socket
