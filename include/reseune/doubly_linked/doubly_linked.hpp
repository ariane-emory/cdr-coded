#ifndef RESEUNE_DOUBLY_LINKED_H
#define RESEUNE_DOUBLY_LINKED_H

#include <cassert>

namespace reseune {
  template <typename T>
  class doubly_linked {
  public:

    // =================================================================================================================
    // Typedefs
    // =================================================================================================================

    using value_type     = T;
    using pointer        = doubly_linked *;
    using reference      = doubly_linked &;
    using const_iterator = linked_list_forward_const_iterator<doubly_linked>;
    
    // =================================================================================================================
    // Member fields
    // =================================================================================================================

    pointer    prev;
    pointer    next;
    value_type data;
    
    // =================================================================================================================
    // Constructors
    // =================================================================================================================

    // onstexpr doubly_linked(): prev(nullptr), next(nullptr), data{} {}
    
    // =================================================================================================================
    // Printing-related member functions
    // =================================================================================================================
    
    inline void describe_instance(char c = '=') const {
      print_line(c);
      print_bits<true, false>("linked list is at", uintptr(this));
      print_bits<true, false>("prev",       uintptr(prev));
      print_bits<true, false>("next",       uintptr(next));
    }

    // =================================================================================================================
    // Other member functions
    // =================================================================================================================
    

#define INSERT(name, A, B)                                                      \
    inline void insert_##name(pointer new_##B) {                                \
      A = new_##B->A;                                                           \
      if (nullptr != A)                                                         \
      A->B = this;                                                              \
      new_##B->A = this;                                                        \
      B = new_##B;                                                              \
    }                                                                           \
                                                                                \
    inline void insert_##name(reference node) {                                 \
      insert_##name(&node);                                                     \
    }

    INSERT(before, prev, next);
    INSERT(after,  next, prev);
    
#undef INSERT
    
    // -----------------------------------------------------------------------------------------------------------------

    inline void remove() {
      if (nullptr != next) next->prev = prev;
      if (nullptr != prev) prev->next = next;

      next = prev = nullptr;
    }

    // =================================================================================================================
    // Iterator-related member functions
    // =================================================================================================================
    
    inline auto begin() const {
      return const_iterator::begin(this);
    }

    inline auto end() const {
      return const_iterator::end();
    }

// =================================================================================================================
  };
}

#endif
