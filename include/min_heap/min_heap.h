/**
 * @file min_heap.h
 * @author libobo (oxox0@qq.com)
 * @brief
 * @version 0.1
 * @date 2022-10-19
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef MIN_HEAP_H
#define MIN_HEAP_H

#include <functional>
#include <ostream>
#include <vector>

template <typename Container, typename Compare = std::less<Container> >
class min_heap {
public:
  typedef std::size_t index_type;
  typedef Container value_type;
  typedef Compare compare_type;
  typedef typename std::vector<value_type>::const_iterator const_iterator;

  min_heap() {}
  ~min_heap() {}

  void push(const value_type &va) {
    heap_.push_back(va);
    up_heap(heap_.size() - 1);
  }

  bool empty() const { return heap_.size() == 0; }

  const_iterator front() const { return heap_.begin(); }

  void pop() {
    if (heap_.size() == 0)
      return;

    std::swap(heap_[0], heap_[heap_.size() - 1]);
    heap_.pop_back();
    down_heap(0);
  }

  void dump(std::ostream &os) {
    os << "{";
    size_t size = heap_.size();
    for (size_t i = 0; i < size - 1; ++i) {
      os << heap_[i] << ", ";
    };
    if (size > 0)
      os << heap_[size - 1];
    os << "}\n";
  }

private:
  void up_heap(index_type index) {
    while (index > 0) {
      index_type parent_index = (index - 1) / 2;
      if (!comp_(heap_[index], heap_[parent_index]))
        break;
      std::swap(heap_[index], heap_[parent_index]);
      index = parent_index;
    }
  }

  void down_heap(index_type index) {
    index_type left = index * 2 + 1;
    while (left < heap_.size()) {
      index_type down_index =
          (left + 1 == heap_.size()) || comp_(heap_[left], heap_[left + 1])
              ? left
              : left + 1;
      if (comp_(heap_[index], heap_[down_index]))
        break;
      std::swap(heap_[index], heap_[down_index]);
      index = down_index;
      left = index * 2 + 1;
    }
  }

private:
  /// parent node is i / 2, left node index 2i, right node index 2i + 1
  std::vector<value_type> heap_;
  compare_type comp_;
};

#endif // MIN_HEAP_H
