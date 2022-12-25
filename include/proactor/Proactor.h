
#ifndef PROACTOR_PROACTOR_H
#define PROACTOR_PROACTOR_H

#include "proactor/operation/detail/Operation.h"

class Proactor {
public:
#ifdef _WIN32
  typedef void *native_handle;
#else  // _WIN32
  typedef pid_t native_handle;
#endif // _WIN32

  Proactor(const Proactor &) = delete;
  const Proactor &operator=(const Proactor &) = delete;

  static Proactor create(std::error_code &ec);

  void shutdown();

  size_t run();
  size_t run_one(size_t timeout_us, std::error_code &ec);

  void stop(std::error_code &ec);

  void notify_op(Operation *op, std::error_code &ec);

  void post(const native_handle &file_descriptor, std::error_code &ec);
  void depose(const native_handle &file_descriptor, std::error_code &ec);

private:
  Proactor();

  native_handle fd_;
  bool shutdown_;
};

#endif // PROACTOR_PROACTOR_H
