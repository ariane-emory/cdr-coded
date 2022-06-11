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
    
    inline void insert_before(pointer new_next) {      
      // next = new_next;
      // if (nullptr != next) prev = next->prev;

      // if (nullptr != next) next->prev = this;
      // if (nullptr != prev) prev->next = this;

      insert_between(new_next->prev, new_next);
    }

    inline void insert_after(pointer new_prev) {
      next = new_prev->next;
      
      if (nullptr != next)
        next->prev = this;
      
      new_prev->next = this;
      prev = new_prev;
    }

    inline void insert_between(pointer new_prev, pointer new_next) {
#define CONNECT(A, B)                                                           \
      if (nullptr == new_ ## A) {                                               \
        A = nullptr;                                                            \
      }                                                                         \
      else {                                                                    \
        A = new_ ## A;                                                          \
        new_ ## A->B = this;                                                    \
      }                                                                         \
     
      CONNECT(prev, next);
      CONNECT(next, prev);
#undef CONNECT
    }

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
