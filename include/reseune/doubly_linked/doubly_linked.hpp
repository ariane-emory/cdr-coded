#ifndef RESEUNE_DOUBLY_LINKED_H
#define RESEUNE_DOUBLY_LINKED_H

#include <cassert>
#include "reseune/linked_list_forward_const_iterator/linked_list_forward_const_iterator.hpp"

namespace reseune {
  template <typename T>
  struct doubly_linked {

    // ===========================================================================================================
    // Typedefs
    // ===========================================================================================================

    using value_type     = T;
    using pointer        = doubly_linked *;
    using reference      = doubly_linked &;
    using const_iterator = linked_list_forward_const_iterator<doubly_linked>;
    using iterator       = linked_list_forward_iterator<doubly_linked>;
    
    // ===========================================================================================================
    // Member fields
    // ===========================================================================================================

    pointer    prev;
    pointer    next;
    value_type data;
    
    // ===========================================================================================================
    // Constructors
    // ===========================================================================================================

    constexpr doubly_linked(pointer p = nullptr, pointer n = nullptr): prev(p), next(n), data{} {
      if (nullptr != prev) prev->next = this;
      if (nullptr != next) next->prev = this;
    }
    
    // ===========================================================================================================
    // Printing-related member functions
    // ===========================================================================================================
    
    inline void describe_instance(char c = '=') const {
      print_line('-');
      print_bits<true, false>("doubly_linked is at", uintptr(this));
      print_bits<true, false>("prev",       uintptr(prev));
      print_bits<true, false>("next",       uintptr(next));
    }

    // ===========================================================================================================
    // Other member functions
    // ===========================================================================================================
    

#define INSERT(name, A, B)                                                      \
    inline void insert_##name(pointer new_##B) {                                \
      A = new_##B->A;                                                           \
      if (nullptr != A)                                                         \
        A->B = this;                                                            \
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
    
    // -----------------------------------------------------------------------------------------------------------

    inline void remove() {
      if (nullptr != next) next->prev = prev;
      if (nullptr != prev) prev->next = next;

      next = prev = nullptr;
    }

    // ===========================================================================================================
    // Iterator-related member functions
    // ===========================================================================================================
    
    inline auto cbegin() const {
      return const_iterator::begin(this);
    }

    inline auto cend() const {
      return const_iterator::end();
    }

    inline auto begin() {
      return iterator::begin(this);
    }

    inline auto end() {
      return iterator::end();
    }

// ===============================================================================================================
  };
}

#endif
