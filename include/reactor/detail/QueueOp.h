
#ifndef REACTOR_DETAIL_QUEUEOP_H
#define REACTOR_DETAIL_QUEUEOP_H

#include "reactor/detail/Operation.h"

class queue_op {
public:
  ~queue_op() {
    while (operation *op = begin_) {
      pop();
    }
    // op->complete(0, std::error_code(), 0); // double free
  }

  operation *begin() const { return begin_; }

  bool empty() const { return begin_ == nullptr; }

  bool exist(operation *op) const {
    return detail::OperationAccess::next((void *)op) != nullptr || end_ == op;
  }

  void pop() {
    if (begin_) {
      operation *tmp = begin_;
      begin_ = static_cast<operation *>(
          detail::OperationAccess::next((void *)begin_));
      if (begin_ == nullptr) {
        end_ = nullptr;
      }
      detail::OperationAccess::set_next((void *)tmp, nullptr);
    }
  }

  void push(operation *op) {
    if (begin_) {
      detail::OperationAccess::set_next((void *)end_, (void *)op);
      end_ = op;
    } else {
      begin_ = end_ = op;
    }
    detail::OperationAccess::set_next((void *)end_, nullptr);
  }

  void push(queue_op &ops) {
    if (begin_) {
      detail::OperationAccess::set_next((void *)end_, (void *)ops.begin_);
      if (ops.end_) {
        end_ = ops.end_;
      }
    } else {
      begin_ = ops.begin_;
      end_ = ops.end_;
    }
    ops.begin_ = nullptr;
    ops.end_ = nullptr;
  }

private:
  operation *begin_{nullptr};
  operation *end_{nullptr};
}; /* class queue_op */

#endif // REACTOR_DETAIL_QUEUEOP_H
