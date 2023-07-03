
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

  void append_task(std::function<void()> f, uint32_t block_size = 256);
  void append_task(std::function<void(coroutine &)> f,
                   uint32_t block_size = 256);

  void yield();
  void loop();
  void clear();
  bool empty();

private:
  void swap(size_t index);

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

    void push_back(const T &t) {
      if (v_size == v.size()) {
        v.push_back(t);
      } else {
        v[v_size] = t;
      }
      ++v_size;
    }

    void push_back(T &&t) {
      if (v_size == v.size()) {
        v.push_back(std::forward<T>(t));
      } else {
        v[v_size] = std::move(t);
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

extern thread_local coroutine STATIC_GLOBOAL_CO_XBREFW;

#define co_yield() STATIC_GLOBOAL_CO_XBREFW.yield()
#define co_await(...) STATIC_GLOBOAL_CO_XBREFW.append_task(__VA_ARGS__)
#define co_loop() STATIC_GLOBOAL_CO_XBREFW.loop()
#define co_stop() STATIC_GLOBOAL_CO_XBREFW.clear()
#define co_empty() STATIC_GLOBOAL_CO_XBREFW.empty()
#define co_loop_call(...)                                                      \
  while (!co_empty()) {                                                        \
    co_yield();                                                                \
    __VA_ARGS__();                                                             \
  }

#endif // COROUTINE_H