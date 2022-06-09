#ifndef RESEUNE_LINKED_LIST_H
#define RESEUNE_LINKED_LIST_H

namespace reseune {
  class link {
  public:
    using pointer = link *;

    pointer next;
    pointer prev;

    void describe_instance() {
      print_line();
      print_bits<true, false>("link is at", reinterpret_cast<uintptr_t>(this));
      print_bits("next", reinterpret_cast<uintptr_t>(next));
      print_bits("prev", reinterpret_cast<uintptr_t>(prev));
    }

    void connect(link * next_, link * prev_) {
      next_->prev = this;
      next        = next_;
      prev        = prev_;
      prev->next  = this;      
    }

    void remove() {
      if (nullptr != next)
        next->prev = prev;
      if (nullptr != prev)
        prev->next = next;
      nnnnext = prev = nullptr;
    }
  };
}

#endif
