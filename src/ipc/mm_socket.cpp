
#include "ipc/mm_socket.h"

#include <cstring>
#include <limits>

#include "utils/random.h"

namespace {

constexpr char SERVER_PREFIX[] = "MMSOCKET";
constexpr size_t CLIENT_SIZE = 1024 * 4096; // 4M

struct Protocal {
  uint32_t id;
  uint32_t ask_id;
  uint32_t status;
};

ipc::SharedMemory open_client(const char *address, uint32_t id) {
  char pre_address[256] = {};
  snprintf(pre_address, sizeof(pre_address), "%s_%s_%X", SERVER_PREFIX, address,
           id);
  std::error_code ec;
  return ipc::SharedMemory::open(pre_address, ec);
}

ipc::SharedMemory create_client(const char *address, size_t mm_size,
                                uint32_t id) {
  char pre_address[256] = {};
  snprintf(pre_address, sizeof(pre_address), "%s_%s_%X", SERVER_PREFIX, address,
           id);
  std::error_code ec;
  return ipc::SharedMemory::create(pre_address, mm_size, ec);
}

ipc::SharedMemory open(const char *address) {
  char pre_address[256] = {};
  snprintf(pre_address, sizeof(pre_address), "%s_%s", SERVER_PREFIX, address);
  std::error_code ec;
  return ipc::SharedMemory::open(pre_address, ec);
}

ipc::SharedMemory create(const char *address, size_t mm_size) {
  char pre_address[256] = {};
  snprintf(pre_address, sizeof(pre_address), "%s_%s", SERVER_PREFIX, address);
  std::error_code ec;
  return ipc::SharedMemory::create(pre_address, mm_size, ec);
}

} // namespace

void mm_client::connect_server(const char *address) {
  close();
  server_handle_ = open(address);
  id_ = randNum();
}

mm_client::mm_client() : id_(0), is_server_(false) {}

int mm_client::connect(const char *address) {
  if (handle_.memory()) {
    return 0; // had connect
  }
  Protocal *pro = (Protocal *)server_handle_.memory();
  if (!pro || id_ == 0) {
    connect_server(address);
    pro = (Protocal *)server_handle_.memory();
    if (!pro) {
      return -1; // no server
    }
  }
  // pre-connect step
  if (pro->id == (decltype(pro->id)) - 1) {
    pro->id = id_;
    return 1; // begin connect server
  }
  if (pro->id != id_) {
    return 2; // wait server completely connect other
  }
  if (pro->ask_id != id_) {
    return 3; // wait server
  }

  // connect
  handle_ = create_client(address, CLIENT_SIZE, id_);
  pro->status = 1;
  std::error_code ec;
  server_handle_.close(ec);
  ec.clear();
  server_handle_.remove(ec);
  return 0;
}

void mm_client::close() {
  std::error_code ec;
  handle_.close(ec);
  ec.clear();
  handle_.remove(ec);
  ec.clear();
  server_handle_.close(ec);
  ec.clear();
  server_handle_.remove(ec);
  id_ = 0;
  is_server_ = false;
}

// @return the send number
int mm_client::send(const char *data, size_t data_size) {
  char *write_mm = (char *)handle_.memory();
  if (!write_mm) {
    return -1;
  }
  if (is_server_) {
    write_mm += (CLIENT_SIZE / 2);
  }
  uint32_t &read_address = *(uint32_t *)write_mm;
  write_mm += sizeof(uint32_t);
  uint32_t &write_address = *(uint32_t *)write_mm;
  write_mm += sizeof(uint32_t);

  uint32_t mm_size = CLIENT_SIZE / 2 - sizeof(uint32_t) * 2;
  int remain_size = mm_size;

  // get remain
  if (read_address != write_address) {
    int rsize = write_address - read_address;
    rsize += (rsize < 0 ? mm_size : 0);
    remain_size -= rsize;
  }
  remain_size -= 1;

  // write data
  size_t write_size = (std::min)((size_t)remain_size, data_size);
  int cover_edge = (write_address + write_size) / mm_size;
  int append_size = (write_address + write_size) % mm_size;
  if (cover_edge > 0) {
    int cover = write_size - append_size;
    memcpy(write_mm + write_address, data, cover);
    memcpy(write_mm, data + cover, append_size);
  } else {
    memcpy(write_mm + write_address, data, write_size);
  }
  write_address = append_size;

  return write_size;
}

// @return the recv number
int mm_client::recv(char *data, size_t data_size) {
  char *read_mm = (char *)handle_.memory();
  if (!read_mm) {
    return -1;
  }
  if (!is_server_) {
    read_mm += (CLIENT_SIZE / 2);
  }

  uint32_t &read_address = *(uint32_t *)read_mm;
  read_mm += sizeof(uint32_t);
  uint32_t &write_address = *(uint32_t *)read_mm;
  read_mm += sizeof(uint32_t);

  uint32_t mm_size = CLIENT_SIZE / 2 - sizeof(uint32_t) * 2;
  int remain_size = 0;

  // get read remain
  if (read_address != write_address) {
    int rsize = write_address - read_address;
    rsize += (rsize < 0 ? mm_size : 0);
    remain_size = rsize;
  }

  // read data
  size_t read_size = (std::min)((size_t)remain_size, data_size);
  int cover_edge = (read_address + read_size) / mm_size;
  int append_size = (read_address + read_size) % mm_size;
  if (cover_edge > 0) {
    int cover = read_size - append_size;
    memcpy(data, read_mm + read_address, cover);
    memcpy(data + cover, read_mm, append_size);
  } else {
    memcpy(data, read_mm + read_address, read_size);
  }
  read_address = append_size;

  return read_size;
}

void mm_socket::init_protocal(void *p) {
  Protocal *pro = (Protocal *)p;
  pro->id = -1;
  pro->ask_id = 0;
  pro->status = 0;
}

int mm_socket::bind(const char *address) {
  std::error_code ec;
  if (bind_handle_.memory()) {
    bind_handle_.close(ec);
    ec.clear();
  }
  bind_handle_ = create(address, 128);
  bind_name_ = address;
  if (ec) {
    // fprintf(stderr, "create mm error %s\n", ec.message().c_str());
    return -1;
  }
  Protocal *pro = (Protocal *)bind_handle_.memory();
  if (!pro) {
    close();
    return -1;
  }
  if (pro->id != 0) {
    close();
    return -1;
  }
  init_protocal(pro);
  return 0;
}

int mm_socket::accept(mm_client &client) {
  Protocal *pro = (Protocal *)bind_handle_.memory();
  if (!pro) {
    return -1; // no bind
  }
  // pre-connect
  if (pro->id == (decltype(pro->id)) - 1) {
    return 1; // no client
  }
  if (!pro->ask_id) {
    pro->ask_id = pro->id;
  }
  if (pro->ask_id != pro->id) {
    return 2;
  }
  if (pro->status != 1) {
    return 3;
  }

  client.close();
  client.handle_ = open_client(bind_name_.c_str(), pro->id);
  client.is_server_ = true;
  client.id_ = pro->ask_id;
  init_protocal(pro);
  return 0;
}

void mm_socket::close() {
  std::error_code ec;
  Protocal *pro = (Protocal *)bind_handle_.memory();
  if (pro) {
    pro->id = 0;
  }
  bind_handle_.close(ec);
  ec.clear();
  bind_handle_.remove(ec);
}
