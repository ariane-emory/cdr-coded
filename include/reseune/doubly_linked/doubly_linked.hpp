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

    constexpr doubly_linked(pointer p = nullptr, pointer n = nullptr): prev(p), next(n), data{} {}
    
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
    

#define INSERT(A,B)                                                             \
    A = new_##B->A;                                                             \
    if (nullptr != A)                                                           \
      A->B = this;                                                              \
    new_##B->A = this;                                                          \
    B = new_##B;

    inline void insert_before(pointer new_next) {      
      INSERT(prev, next);
    }

    inline void insert_after(pointer new_prev) {
      INSERT(next, prev);
    }
    
#undef INSERT
    
    // -----------------------------------------------------------------------------------------------------------------

#define INSERT_WITH_REFERENCE(position)                                         \
    inline void insert_ ## position(reference node) {                           \
      insert_ ## position(&node);                                               \
    }

    INSERT_WITH_REFERENCE(before);
    INSERT_WITH_REFERENCE(after);

#undef INSERT_WITH_REFERENCE

// inline void insert_before(reference new_next) {
//   insert_before(&new_next);
// }
// inline void insert_after(reference new_prev) {
//   insert_after(&new_prev);
// }

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
