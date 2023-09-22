
#ifdef _WIN32

#include "coroutine/coroutine.h"

#include "windows.h"
#include <stdio.h>

struct context {
  std::function<void()> callback;
  LPVOID fiber;

  context() : fiber(nullptr) {}
  ~context() {} /*free_stack();*/

  void make_context(context &end_ctx, std::function<void()> f,
                    uint32_t block_size) {

    constexpr uint32_t BLOCK_SIZE = 4096;

    callback = [&end_ctx, f]() {
      f();
      end_ctx.swap_context(end_ctx);
    };
    fiber =
        CreateFiber(BLOCK_SIZE * block_size,
                    (LPFIBER_START_ROUTINE)swap_function, (void *)&callback);
    if (!fiber) {
      fprintf(stderr, "CreateFiber error %d\n", GetLastError());
    }
  }

  void swap_context(context &next) {
    if (!fiber) {
      fiber = ConvertThreadToFiber(nullptr);
      if (!fiber) {
        fiber = GetCurrentFiber();
        fprintf(stderr, "ConvertThreadToFiber error %d\n", GetLastError());
      }
    }
    SwitchToFiber(next.fiber);
  }

  void reset() {
    callback = std::function<void()>();
    free_stack();
  }

private:
  void free_stack() {
    if (fiber) {
      DeleteFiber(fiber);
      fiber = nullptr;
    }
  }

  static void swap_function(void *f) {
    if (!f) {
      return;
    }
    std::function<void()> &c = *reinterpret_cast<std::function<void()> *>(f);
    if (c) {
      c();
    }
  }
};

#endif // _WIN32
