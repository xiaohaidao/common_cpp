
#ifndef IPC_MM_SOCKET_H
#define IPC_MM_SOCKET_H

#include <cstdint>

#include "ipc/SharedMemory.h"

class mm_client {
  friend class mm_socket;

public:
  mm_client();

  int connect(const char *address);

  void close();

  // @return the send number
  int send(const char *data, size_t data_size);

  // @return the recv number
  int recv(char *data, size_t data_size);

private:
  void connect_server(const char *address);

  uint32_t id_;
  bool is_server_;
  ipc::SharedMemory server_handle_;
  ipc::SharedMemory handle_;

}; // class mm_client

class mm_socket {
public:
  int bind(const char *address);

  int accept(mm_client &client);

  void close();

private:
  void init_protocal(void *pro);

  ipc::SharedMemory bind_handle_;
  std::string bind_name_;

}; // class mm_socket

#endif // IPC_MM_SOCKET_H
