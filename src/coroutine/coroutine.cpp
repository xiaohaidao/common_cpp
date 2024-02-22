
#include "coroutine/coroutine.h"

#include "coroutine_posix.h"
#include "coroutine_win.h"

thread_local std::unique_ptr<coroutine> static_globoal_co_xbrefw;

coroutine &get_global_coroutine() {
  if (!static_globoal_co_xbrefw) {
    static_globoal_co_xbrefw = std::unique_ptr<coroutine>(new coroutine);
  }
  return *static_globoal_co_xbrefw;
}

coroutine::coroutine() : current_index_(0), is_exit_to_main_(true) {
  contexts_.push_back(std::make_shared<context>());
}

coroutine::~coroutine() {
  loop();
  clear();
}

void coroutine::append_task(const std::function<void()> &f,
                            uint32_t block_size) {
  if (contexts_.push_back() == nullptr) {
    contexts_.push_back(std::make_shared<context>());
  }
  auto &ctx = contexts_[contexts_.size() - 1];
  ctx->reset();
  ctx->make_context(*contexts_[0],
                    [this, f]() {
                      f();
                      is_exit_to_main_ = true;
                    },
                    block_size);
}

void coroutine::append_task(const std::function<void(coroutine &)> &f,
                            uint32_t block_size) {
  if (contexts_.push_back() == nullptr) {
    contexts_.push_back(std::make_shared<context>());
  }
  auto &ctx = contexts_[contexts_.size() - 1];
  ctx->reset();
  ctx->make_context(*contexts_[0],
                    [this, f]() {
                      f(*this);
                      is_exit_to_main_ = true;
                    },
                    block_size);
}

void coroutine::yield() {
  swap(is_exit_to_main_ ? current_index_ : current_index_ + 1);
}

// NOLINTNEXTLINE(misc-no-recursion)
void coroutine::swap(size_t index) {
  if (contexts_.size() <= 1) {
    return;
  }
  if (current_index_ && is_exit_to_main_) {
    contexts_.erase(current_index_);
    --current_index_;
  }
  size_t last_index = current_index_;
  current_index_ = index;
  if (current_index_ >= contexts_.size()) {
    current_index_ = 0;
  }
  if (is_exit_to_main_) {
    last_index = 0;
  }
  is_exit_to_main_ = false;
  if (last_index == current_index_) {
    return swap(current_index_ + 1);
  }
  contexts_[last_index]->swap_context(*(contexts_[current_index_]));
}

void coroutine::loop() {
  while (!empty()) {
    yield();
  }
}

void coroutine::clear() {
  while (contexts_.push_back() != nullptr) {
  }
  while (!empty()) {
    contexts_[contexts_.size() - 1]->reset();
    contexts_.erase(contexts_.size() - 1);
  }
}

bool coroutine::empty() { return contexts_.size() <= 1; }