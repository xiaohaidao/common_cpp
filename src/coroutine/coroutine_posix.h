
#ifdef __linux__

#include "coroutine/coroutine.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <ucontext.h>

struct context {
  ucontext_t ctx;
  void *stack;
  uint32_t stack_size;
  std::function<void()> callback;

  context() : ctx({}), stack(nullptr), stack_size(0) {}
  ~context() { free_stack(); }

  void make_context(context &end_ctx, std::function<void()> f,
                    uint32_t block_size) {
    if (::getcontext(&ctx) < 0) {
      fprintf(stderr, "make context error %s\n", strerror(errno));
      return;
    }

    new_stack(block_size);
    if (!stack_size) {
      return;
    }

    callback = f;
    ctx.uc_stack.ss_sp = stack;
    ctx.uc_stack.ss_size = stack_size;
    ctx.uc_link = &(end_ctx.ctx);

    ::makecontext(&ctx, (void (*)())swap_function, 1, (void *)&callback);
  }

  void swap_context(context &next) {
    if (::swapcontext(&ctx, &next.ctx) < 0) {
      fprintf(stderr, "swap context error %s\n", strerror(errno));
    }
  }

  void reset() { callback = std::function<void()>(); }

private:
  void free_stack() {
    if (stack) {
      free(stack);
    }
    stack = nullptr;
    stack_size = 0;
  }

  void new_stack(uint32_t block_size) {
    if (block_size == 0) {
      return;
    }
    constexpr uint32_t BLOCK_SIZE = 4096;
    size_t size = BLOCK_SIZE * block_size;
    if (stack_size == size) {
      return;
    }
    if (stack_size > 0) {
      free_stack();
    }
    stack_size = size;
    if (stack_size) {
      stack = malloc(stack_size);
    }
  }

  static void swap_function(void *f) {
    if (!f) {
      return;
    }
    std::function<void()> &c = *reinterpret_cast<std::function<void()> *>(f);
    if (c) {
      auto tmp = std::move(c);
      tmp();
    }
  }
};

#endif // __linux__
