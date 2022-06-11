#ifndef RESEUNE_SINGLY_LINKED_H
#define RESEUNE_SINGLY_LINKED_H

#include <cassert>

namespace reseune {
  template <typename T>
  class singly_linked {
  public:

    // ===========================================================================================================
    // Typedefs
    // ===========================================================================================================

    using value_type     = T;
    using pointer        = singly_linked *;
    using reference      = singly_linked &;
    using const_iterator = linked_list_forward_const_iterator<singly_linked>;
    
    // ===========================================================================================================
    // Member fields
    // ===========================================================================================================

    pointer    next;
    value_type data;

    // ===========================================================================================================
    // Constructors
    // ===========================================================================================================

    // constexpr singly_linked(): next(nullptr), data{} {}
    
    // ===========================================================================================================
    // Printing-related member functions
    // ===========================================================================================================
    
    inline void describe_instance() const {
      print_line();
      print_bits<true, false>("singly_linked is at", uintptr(this));
      print_bits<true, false>("next",       uintptr(next));
    }

    // ===========================================================================================================
    // Other member functions
    // ===========================================================================================================
    
    inline void insert_before(pointer next_) {
      next = next_;
    }
    
    inline void insert_before(reference next_) {
      insert_before(&next_);
    }

    // ===========================================================================================================
    // Iterator-related member functions
    // ===========================================================================================================
    
    inline auto begin() const {
      return const_iterator::begin(this);
    }

    inline auto end() const {
      return const_iterator::end();
    }
    
    // ===========================================================================================================
  };
}

#endif
