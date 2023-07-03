
#include "coroutine/coroutine.h"

#include "coroutine_posix.h"
#include "coroutine_win.h"

thread_local coroutine STATIC_GLOBOAL_CO_XBREFW;

coroutine::coroutine() : current_index_(0), is_exit_to_main_(true) {
  contexts_.push_back(std::move(std::make_shared<context>()));
}

coroutine::~coroutine() { loop(); }

void coroutine::append_task(std::function<void()> f, uint32_t block_size) {
  if (contexts_.push_back() == nullptr) {
    contexts_.push_back(std::move(std::make_shared<context>()));
  }
  std::shared_ptr<context> &ctx = contexts_[contexts_.size() - 1];
  ctx->make_context(*contexts_[0],
                    [this, f]() {
                      f();
                      is_exit_to_main_ = true;
                    },
                    block_size);
}

void coroutine::append_task(std::function<void(coroutine &)> f,
                            uint32_t block_size) {
  if (contexts_.push_back() == nullptr) {
    contexts_.push_back(std::move(std::make_shared<context>()));
  }
  std::shared_ptr<context> &ctx = contexts_[contexts_.size() - 1];
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

void coroutine::swap(size_t index) {
  if (contexts_.size() <= 1) {
    return;
  }
  if (current_index_ && is_exit_to_main_) {
    contexts_[current_index_]->reset();
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
  while (!empty()) {
    contexts_.erase(contexts_.size() - 1);
  }
}

bool coroutine::empty() { return contexts_.size() <= 1; }