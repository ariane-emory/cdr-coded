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
  };
}

#endif
