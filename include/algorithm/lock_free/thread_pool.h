// Copyright (C) 2021 All rights reserved.
// Email: oxox0@qq.com. Created in 202105

#ifndef ALGORITHM_LOCK_FREE_THREAD_POOL_H
#define ALGORITHM_LOCK_FREE_THREAD_POOL_H

#include <deque>
#include <future>
#include <thread>
#include <type_traits>

#include "threadsafe_queue.h"

namespace thread {

class thread_pool {
  class function_wrapper {
    struct impl_base {
      virtual void call() = 0;
      virtual ~impl_base() = default;
    };

    std::unique_ptr<impl_base> impl_;
    template <typename F> struct impl_type : impl_base {
      F f;
      impl_type(F &&f) : f(std::move(f)) {}
      void call() override { f(); }
    };

  public:
    template <typename F, typename = typename std::enable_if<
                              std::is_class<F>::value, void>::type>
    function_wrapper(F &&f) : impl_(new impl_type<F>(std::forward<F>(f))) {}
    void operator()() { impl_->call(); }
    function_wrapper() = default;
    function_wrapper(function_wrapper &&other) = default;
    function_wrapper &operator=(function_wrapper &&other) = default;

    function_wrapper(const function_wrapper &) = delete;
    function_wrapper(function_wrapper &) = delete;
    function_wrapper &operator=(const function_wrapper &) = delete;
  };

  class work_stealing_queue {
    using data_type = function_wrapper;
    std::deque<data_type> the_queue_;
    mutable std::mutex the_mutex_;

  public:
    work_stealing_queue() = default;
    work_stealing_queue(const work_stealing_queue &other) = delete;
    work_stealing_queue &operator=(const work_stealing_queue &other) = delete;

    void push(data_type data) {
      std::lock_guard<std::mutex> lock(the_mutex_);
      the_queue_.push_front(std::move(data));
    }

    bool empty() const {
      std::lock_guard<std::mutex> lock(the_mutex_);
      return the_queue_.empty();
    }

    bool try_pop(data_type &res) {
      std::lock_guard<std::mutex> lock(the_mutex_);
      if (the_queue_.empty()) {
        return false;
      }
      res = std::move(the_queue_.front());
      the_queue_.pop_front();
      return true;
    }
    bool try_steal(data_type &res) {
      std::lock_guard<std::mutex> lock(the_mutex_);
      if (the_queue_.empty()) {
        return false;
      }
      res = std::move(the_queue_.back());
      the_queue_.pop_back();
      return true;
    }
  };

  using task_type = function_wrapper;

  std::atomic_bool done_;
  threadsafe_queue<task_type> pool_work_queue_;
  std::vector<std::unique_ptr<work_stealing_queue> > queues_;
  std::vector<std::thread> threads_;
  // join_threads joiner;

  static work_stealing_queue *&local_work_queue() {
    thread_local work_stealing_queue *q;
    return q;
  }
  static size_t &my_index() {
    thread_local size_t i;
    return i;
  }

  void work_thread(uint32_t index) {
    my_index() = index;
    local_work_queue() = queues_[my_index()].get();
    while (!done_) {
      run_pending_task();
    }
  }

  bool pop_task_from_local_queue(task_type &task) {
    return local_work_queue() && local_work_queue()->try_pop(task);
  }
  bool pop_task_from_pool_queue(task_type &task) {
    return pool_work_queue_.try_pop(task);
  }
  bool pop_task_from_other_thread_queue(task_type &task) {
    size_t queues_size = queues_.size();
    for (size_t i = 0; i < queues_size; ++i) {
      size_t index = (my_index() + i + 1) % queues_size;
      if (!queues_[index]) {
        break;
      }
      if (queues_[index]->try_steal(task)) {
        return true;
      }
    }
    return false;
  }

public:
  explicit thread_pool(uint32_t thread_number = 0) : done_(false) {
    uint32_t thread_count = std::thread::hardware_concurrency();
    if (thread_number != 0 && thread_number < thread_count) {
      thread_count = thread_number;
    }
    try {
      queues_.resize(thread_count);
      threads_.resize(thread_count);
      for (uint32_t i = 0; i < thread_count; ++i) {
        queues_[i] = std::move(
            std::unique_ptr<work_stealing_queue>(new work_stealing_queue));

        threads_[i] =
            std::move(std::thread(&thread_pool::work_thread, this, i));
      }
    } catch (...) {
      done_ = true;
      throw;
    }
  }
  ~thread_pool() {
    done_ = true;
    for (auto &i : threads_) {
      i.join();
    }
  }

  template <typename FunctionType>
  std::future<typename std::result_of<FunctionType()>::type>
  submit(FunctionType f) {
#if __cplusplus < 201703L // C++17
    using result_type = typename std::result_of<FunctionType()>::type;
#else
    using result_type = typename std::invoke_result_t<FunctionType>;
#endif

    std::packaged_task<result_type()> task(std::move(f));
    std::future<result_type> res(task.get_future());
    if (local_work_queue()) {
      local_work_queue()->push(std::move(task));
    } else {
      pool_work_queue_.push(std::move(task));
    }
    return res;
  }

  size_t thread_size() const { return threads_.size(); }

  void run_pending_task() {
    task_type task;
    if (pop_task_from_local_queue(task) || pop_task_from_pool_queue(task) ||
        pop_task_from_other_thread_queue(task)) {

      task();
    } else {
      std::this_thread::yield();
    }
  }
};

} // namespace thread

#endif // ALGORITHM_LOCK_FREE_THREAD_POOL_H
