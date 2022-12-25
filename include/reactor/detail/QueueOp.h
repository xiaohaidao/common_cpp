
#ifndef REACTOR_DETAIL_QUEUEOP_H
#define REACTOR_DETAIL_QUEUEOP_H

#include "reactor/detail/ReactorOp.h"

class QueueOp {
public:
  QueueOp() : begin_(nullptr), end_(nullptr) {}
  ~QueueOp() {
    while (ReactorOp *op = begin_) {
      pop();
      op->complete(0, std::error_code(), 0);
    }
  }

  ReactorOp *begin() const { return begin_; }

  bool empty() const { return begin_ == nullptr; }

  bool exist(ReactorOp *op) const {
    return detail::OperationAccess::next((void *)op) != nullptr || end_ == op;
  }

  void pop() {
    if (begin_) {
      ReactorOp *tmp = begin_;
      begin_ = static_cast<ReactorOp *>(
          detail::OperationAccess::next((void *)begin_));
      if (begin_ == nullptr) {
        end_ = nullptr;
      }
      detail::OperationAccess::set_next((void *)tmp, nullptr);
    }
  }

  void push(ReactorOp *op) {
    if (begin_) {
      detail::OperationAccess::set_next((void *)end_, (void *)op);
      end_ = op;
    } else {
      begin_ = end_ = op;
    }
    detail::OperationAccess::set_next((void *)end_, nullptr);
  }

  void push(QueueOp &ops) {
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
  ReactorOp *begin_;
  ReactorOp *end_;
}; // class QueueOp

#endif // REACTOR_DETAIL_QUEUEOP_H
