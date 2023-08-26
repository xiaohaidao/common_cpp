/**
 * @file timer_task.h
 * @author libobo (oxox0@qq.com)
 * @brief
 * @date 2022-10-21
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef TIMER_TASK_H
#define TIMER_TASK_H

#include <cassert>
#include <chrono>
#include <condition_variable>
#include <map>
#include <mutex>
#include <thread>

#include "min_heap.h"

template <typename Key> class timer_task {
public:
  timer_task() : shutdown_(false), thread_(&timer_task::run, this) {}

  ~timer_task() {
    shutdown_ = true;
    shutdown();
    thread_.join();
  }

  template <typename F> void post(const Key &handle, uint32_t time_ms, F f) {
    push(handle, time_ms, f);
  }

  void shutdown() { clearAllOp(); }

  bool empty() { return heap_.empty(); }

private:
  void push(const Key &handle, uint32_t time_ms, std::function<void()> f) {
    auto timeout = time_type::clock::now() + std::chrono::milliseconds(time_ms);
    std::lock_guard<std::mutex> lock(lck_);
    erase(handle);
    delay_task_[handle] = {timeout, f};
    delay_time_[timeout] = handle;
    heap_.push(timeout);
    cv_.notify_all();
  }

  void erase(const Key &handle) {
    if (delay_task_.find(handle) == delay_task_.end()) {
      return;
    }
    auto pair = delay_task_[handle];
    delay_task_.erase(handle);
    delay_time_.erase(pair.first);
    heap_.erase(pair.first);
  }

  std::chrono::microseconds getReadyTimeout(
      const std::chrono::microseconds &max_timeout = std::chrono::minutes(15)) {
    if (heap_.empty())
      return max_timeout;
    auto min_timer = *heap_.front();
    auto now = time_type::clock::now();
    auto min_timeout =
        std::chrono::duration_cast<std::chrono::microseconds>(min_timer - now);
    return std::min(min_timeout, max_timeout);
  }

  void clearAllOp() {
    std::lock_guard<std::mutex> lock(lck_);
    delay_task_.clear();
    delay_time_.clear();
    while (!heap_.empty())
      heap_.pop();
    cv_.notify_all();
  }

  std::vector<std::function<void()> > getReadyOp() {
    std::vector<std::function<void()> > ops;
    auto now = time_type::clock::now();
    while (!heap_.empty() && *heap_.front() <= now) {
      auto const &ready_time = *heap_.front();
      assert(delay_time_.find(ready_time) != delay_time_.end());
      const Key &handle = delay_time_[ready_time];
      assert(delay_task_.find(handle) != delay_task_.end());
      ops.push_back(delay_task_[handle].second);
      delay_task_.erase(handle);
      delay_time_.erase(ready_time);
      heap_.pop();
    }
    return ops;
  }

  void run() {
    while (!shutdown_) {
      std::unique_lock<std::mutex> lock(lck_);
      auto min_time_us = getReadyTimeout();
      if (min_time_us > std::chrono::microseconds(0) && !shutdown_) {
        cv_.wait_for(lock, min_time_us);
      }
      auto ops = getReadyOp();
      lock.unlock();
      for (auto &op : ops) {
        op();
      }
    }
  }

private:
  std::mutex lck_;
  std::condition_variable cv_;
  bool shutdown_;
  std::thread thread_;

  typedef std::chrono::time_point<std::chrono::steady_clock> time_type;
  min_heap<time_type> heap_;
  // <index, func>
  std::map<Key, std::pair<time_type, std::function<void()> > > delay_task_;
  // <time, index>
  std::map<time_type, Key> delay_time_;
};

#endif // TIMER_TASK_H
