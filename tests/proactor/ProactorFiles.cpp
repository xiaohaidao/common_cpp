
#include "gtest/gtest.h"

#include <array>

#include "ipc/Pipe.h"
#include "proactor/Proactor.h"
#include "proactor/operation/FilesOp.h"
#include "utils/log.h"

using namespace std::placeholders; // for _1, _2, _3...

class Files {
public:
  Files(Proactor &p, const char *module, native_handle s)
      : buff_{}, file_op_(&p, s), module_(module) {}
  ~Files() {}

  ::native_handle native_handle() const { return file_op_.native_handle(); }

  void close() {
    LOG_TRACE("module: %s, close fd %d", module_.c_str(), native_handle());
    std::error_code ec;
    file_op_.close(ec);
    EXPECT_FALSE(ec) << "module: " << module_ << ", " << ec.value() << " : "
                     << ec.message();
  }

  void read(const std::error_code &re_ec, size_t size, Files *f) {
    EXPECT_FALSE(re_ec) << "module: " << module_ << ", " << re_ec.value()
                        << " : " << re_ec.message();
    LOG_TRACE("%s: %d async read size %d %d \"%s\"", module_.c_str(),
              native_handle(), size, strlen(buff_), buff_);
    if (!re_ec) {
      f->async_write(buff_, size, this);
    }
  }

  void write(const std::error_code &re_ec, size_t size, Files *f) {
    EXPECT_FALSE(re_ec) << "module: " << module_ << ", " << re_ec.value()
                        << " : " << re_ec.message();
    LOG_TRACE("%s: %d async write buff complete %d %d \"%s\"", module_.c_str(),
              native_handle(), size, strlen(buff_), buff_);
    if (!re_ec) {
      f->async_read(this);
    }
  }

  void async_read(Files *f) {
    memset(buff_, 0, sizeof(buff_));
    std::error_code ec;
    file_op_.async_read((char *)buff_, sizeof(buff_),
                        std::bind(&Files::read, this, _1, _2, f), ec);
    EXPECT_FALSE(ec) << "module: " << module_ << ", " << ec.value() << " : "
                     << ec.message();
  }

  void async_write(const char *buff, size_t size, Files *f) {
    size = (std::min)(size, sizeof(buff_));
    memcpy(buff_, buff, size);
    buff_[size] = 0;
    LOG_TRACE("%s: write message \"%s\"", module_.c_str(), buff_);
    std::error_code ec;
    file_op_.async_write((char *)buff_, size,
                         std::bind(&Files::write, this, _1, _2, f), ec);
    EXPECT_FALSE(ec) << "module: " << module_ << ", " << ec.value() << " : "
                     << ec.message();
  }

  char buff_[1024];
  FilesOp file_op_;
  std::string module_;
};

/*
TEST(ProactorTest, ProactorFiles) {
  std::error_code ec;
  Proactor p(ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  char path[] = "test_pipe_name";
  LOG_TRACE("op file %s", path);

  using ipc::Pipe;
  Pipe pipe = Pipe::create(path, ec);
  if (ec) {
    EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
    ec.clear();
    pipe = Pipe::connect(path, ec);
  }
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  Pipe client_pipe = Pipe::connect(path, ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  Files server(p, "Server", pipe.read_native());
  Files client(p, "client", client_pipe.write_native());

  LOG_TRACE("async read");
  server.async_read(&client);
  LOG_TRACE("async read complete");

  char buff[] = "client file write message!";
  client.async_write(buff, sizeof(buff), &server);

  LOG_TRACE("-------------------- begin run while --------------------");
  for (size_t i = 0; i < 10; ++i) {
    p.run_one(1000 * 1000, ec);
    EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
    ec.clear();
  }
  LOG_TRACE("-------------------- end run while --------------------");
  client.close();
  server.close();

  pipe.close(ec);
  // EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  p.close(ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
}
*/
