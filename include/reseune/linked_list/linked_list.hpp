#ifndef RESEUNE_LINKED_LIST_H
#define RESEUNE_LINKED_LIST_H

#include <cassert>

namespace reseune {
  class link {
  public:
    using pointer   = link *;
    using reference = link &;

    pointer prev;
    pointer next;

    inline link(): prev(nullptr), next(nullptr) {}
    
    inline void describe_instance() {
      print_line();
      print_bits<true, false>("link is at", reinterpret_cast<uintptr_t>(this));
      print_bits<true, false>("next", reinterpret_cast<uintptr_t>(next));
      print_bits<true, false>("prev", reinterpret_cast<uintptr_t>(prev));
    }

    inline void connect(pointer next_, pointer prev_) {
      if (nullptr != next) next_->prev = this;
      next        = next_;
      prev        = prev_;
      if (nullptr != prev) prev->next  = this;      
    }

    inline void insert_before(pointer next_) {
      assert(nullptr != next_);

      prev = next_->prev;
      next = next_;
      next->prev = this;
      if (nullptr != prev) prev->next = this;
    }
    inline void insert_before(reference next_) {
      insert_before(&next_);
    }

    inline void insert_after(pointer prev_) {
      assert(nullptr != prev_);

      prev = prev_;
      next = prev_->next;
      if (nullptr != next) next->prev = this;
      prev->next = this;
    }
    inline void insert_after(reference prev_) {
      insert_after(&prev_);
    }

    inline void remove() {
      if (nullptr != next) next->prev = prev;
      if (nullptr != prev) prev->next = next;

      next = prev = nullptr;
    }
  };
}

#endif
