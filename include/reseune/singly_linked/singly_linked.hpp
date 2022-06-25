#ifndef RESEUNE_SINGLY_LINKED_H
#define RESEUNE_SINGLY_LINKED_H

#include <cassert>

#include "../linked_list_forward_const_iterator/linked_list_forward_const_iterator.hpp"

namespace reseune {
  template <typename T>
  struct singly_linked {
    // =================================================================================================================
    // Typedefs
    // =================================================================================================================
    using value_type     = T;
    using pointer        = singly_linked *;
    using reference      = singly_linked &;
    using const_iterator = linked_list_forward_const_iterator<singly_linked>;
    
    // =================================================================================================================
    // Constructor
    // =================================================================================================================
    singly_linked(const value_type & d, singly_linked * n = nullptr) :
      data(d), next(n) {}
    
    // =================================================================================================================
    // Member fields
    // =================================================================================================================
    value_type data;
    pointer    next;

    // =================================================================================================================
    // Printing-related member functions
    // =================================================================================================================
    constexpr void describe_instance() const {
      print_line();
      print_bits<true, false>("singly_linked is at", uintptr(this));
      print_bits<true, false>("next",       uintptr(next));
    }

    // =================================================================================================================
    // Other member functions
    // =================================================================================================================
    constexpr void insert_before(pointer next_) {
      next = next_;
    }
    
    constexpr void insert_before(reference next_) {
      insert_before(&next_);
    }

    // =================================================================================================================
    // Iterator-related member functions
    // =================================================================================================================
    constexpr auto begin() const {
      return const_iterator::begin(this);
    }

    constexpr auto end() const {
      return const_iterator::end();
    }
    
    // =================================================================================================================
  };
  // ===================================================================================================================
}
// =====================================================================================================================
#endif
