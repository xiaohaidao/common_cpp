
#ifndef COROUTINE_H
#define COROUTINE_H

#include <functional>
#include <memory>
#include <vector>

struct context;

class coroutine {
public:
  coroutine();
  ~coroutine();

  coroutine(coroutine &) = delete;
  coroutine &operator=(coroutine &) = delete;

  void append_task(const std::function<void()> &f, uint32_t block_size = 256);
  void append_task(const std::function<void(coroutine &)> &f,
                   uint32_t block_size = 256);

  void yield();
  void loop();
  bool empty();

private:
  void swap(size_t index);
  void clear();

  template <typename T> struct vec {
    size_t v_size;
    std::vector<T> v;
    vec() : v_size(0) {}

    typename std::vector<T>::pointer push_back() {
      if (size() >= v.size()) {
        return nullptr;
      }
      return &v[v_size++];
    }

    void push_back(T &&t) {
      if (v_size == v.size()) {
        v.emplace_back(std::forward<T>(t));
      } else {
        v[v_size] = std::forward<T>(t);
      }
      ++v_size;
    }

    typename std::vector<T>::reference operator[](size_t index) {
      return v[index];
    }

    size_t size() { return v_size; }

    void erase(const T &value) {
      for (auto &i : v) {
        if (i == value) {
          std::swap(i, v[--v_size]);
          break;
        }
      }
    }

    void erase(size_t index) {
      if (index >= v_size) {
        return;
      }
      std::swap(v[index], v[--v_size]);
    }
  };

  vec<std::shared_ptr<context> > contexts_;
  size_t current_index_;
  bool is_exit_to_main_;

}; // class coroutine

coroutine &get_global_coroutine();

#define co_yield() get_global_coroutine().yield()
#define co_await(...) get_global_coroutine().append_task(__VA_ARGS__)
#define co_loop() get_global_coroutine().loop()
#define co_empty() get_global_coroutine().empty()
#define co_loop_call(...)                                                      \
  while (!co_empty()) {                                                        \
    co_yield();                                                                \
    __VA_ARGS__();                                                             \
  }

#endif // COROUTINE_H
