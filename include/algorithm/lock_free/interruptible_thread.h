// Copyright (C) 2021 All rights reserved.
// Email: oxox0@qq.com. Created in 202105

#ifndef ALGORITHM_LOCK_FREE_INTERRUPTIBLE_THREAD_H
#define ALGORITHM_LOCK_FREE_INTERRUPTIBLE_THREAD_H

#include <atomic>
#include <condition_variable>
#include <future>
#include <mutex>
#include <stdexcept>
#include <thread>

namespace thread {
void interruption_point();

namespace impl {

class interrupt_flag {
  std::atomic<bool> flag_;
  std::condition_variable *thread_cond_{nullptr};
  std::condition_variable_any *thread_cond_any_{nullptr};
  std::mutex set_clear_mutex_;

public:
  interrupt_flag() : flag_(false) {}
  void set() {
    flag_.store(true, std::memory_order_relaxed);
    std::lock_guard<std::mutex> lk(set_clear_mutex_);
    if (thread_cond_) {
      thread_cond_->notify_all();
    } else if (thread_cond_any_) {
      thread_cond_any_->notify_all();
    }
  }

  bool is_set() const { return flag_.load(std::memory_order_relaxed); }

  void set_condition_variable(std::condition_variable &cv) {
    std::lock_guard<std::mutex> lk(set_clear_mutex_);
    thread_cond_ = &cv;
  }

  void clear_condition_variable() {
    std::lock_guard<std::mutex> lk(set_clear_mutex_);
    thread_cond_ = nullptr;
  }

  template <typename Lockable>
  void wait(std::condition_variable_any &cv, Lockable &lk) {
    class custom_lock {
      interrupt_flag *self_;
      Lockable &lk_;

    public:
      custom_lock(interrupt_flag *self, std::condition_variable_any &cond,
                  Lockable &lk)
          : self_(self), lk_(lk) {

        self_->set_clear_mutex_.lock();
        self_->thread_cond_any_ = &cond;
      }

      void unlock() {
        lk_.unlock();
        self_->set_clear_mutex_.unlock();
      }

      void lock() { std::lock(self_->set_clear_mutex_, lk_); }

      ~custom_lock() {
        self_->thread_cond_any_ = nullptr;
        self_->set_clear_mutex_.unlock();
      }
    };
    custom_lock cl(this, cv, lk);
    interruption_point();
    cv.wait(cl);
    interruption_point();
  }
};

static thread_local interrupt_flag this_thread_interrupt_flag;

} // namespace impl

inline void interruption_point() {
  if (impl::this_thread_interrupt_flag.is_set()) {
    // throw thread_intterrupted();
    throw std::runtime_error("thread_intterrupted");
  }
}

template <typename Lockable>
void interruptible_wait(std::condition_variable_any &cv, Lockable &lk) {

  impl::this_thread_interrupt_flag.wait(cv, lk);
}

template <typename T> void interruptible_wait(std::future<T> &uf) {
  while (!impl::this_thread_interrupt_flag.is_set()) {
    if (std::future_status::ready ==
        uf.wait_for(std::chrono::milliseconds(1))) {

      break;
    }
  }
  interruption_point();
}

class interruptible_thread {
  std::thread internal_thread_;
  impl::interrupt_flag *flag_;

public:
  template <typename FunctionType>
  explicit interruptible_thread(FunctionType f) {
    std::promise<impl::interrupt_flag *> p;
    internal_thread_ = std::thread([f, &p] {
      p.set_value(&impl::this_thread_interrupt_flag);
      try {
        f();
      } catch (std::exception const &) {
      }
    });
    flag_ = p.get_future().get();
  }

  void join() { internal_thread_.join(); }
  void detach() { internal_thread_.detach(); }
  bool joinable() const { return internal_thread_.joinable(); }
  void interrupt() {
    if (flag_) {
      flag_->set();
    }
  }
};

} // namespace thread

#endif // ALGORITHM_LOCK_FREE_INTERRUPTIBLE_THREAD_H
