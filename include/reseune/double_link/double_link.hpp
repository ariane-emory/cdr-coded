#ifndef RESEUNE_DOUBLE_LINK_H
#define RESEUNE_DOUBLE_LINK_H

#include <cassert>

namespace reseune {
  class double_link {
  public:

    // =================================================================================================================
    // Typedefs
    // =================================================================================================================
    
    using pointer        = double_link *;
    using reference      = double_link &;
    using const_iterator = linked_list_forward_const_iterator<double_link>;
    
    // =================================================================================================================
    // Member fields
    // =================================================================================================================

    pointer prev;
    pointer next;

    // =================================================================================================================
    // Constructors
    // =================================================================================================================

    constexpr double_link(): prev(nullptr), next(nullptr) {}
    
    // =================================================================================================================
    // Printing-related member functions
    // =================================================================================================================
    
    inline void describe_instance() const {
      print_line();
      print_bits<true, false>("link is at", reinterpret_cast<uintptr_t>(this));
      print_bits<true, false>("prev",       reinterpret_cast<uintptr_t>(prev));
      print_bits<true, false>("next",       reinterpret_cast<uintptr_t>(next));
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
      // prev = new_prev;
      // if (nullptr != prev) next = prev->next;
      
      // if (nullptr != next) next->prev = this;
      // if (nullptr != prev) prev->next = this;

      insert_between(new_prev->next, new_prev);
    }

    inline void insert_between(pointer new_prev, pointer new_next) {
      if (nullptr == new_prev) {
        prev = nullptr;
      }
      else {
        prev = new_prev;
        new_prev->next = this;
      }
     
      if (nullptr == new_next) {
        next = nullptr;
      }
      else {
        next = new_next;
        new_next->prev = this;
      }
    }

    // -----------------------------------------------------------------------------------------------------------------

    inline void insert_before(reference new_next) {
      insert_before(&new_next);
    }
    inline void insert_after(reference new_prev) {
      insert_after(&new_prev);
    }

    // -----------------------------------------------------------------------------------------------------------------

    inline void remove() {
      if (nullptr != next) next->prev = prev;
      if (nullptr != prev) prev->next = next;

      next = prev = nullptr;
    }

    // =================================================================================================================
    // Iterator-related member functions
    // =================================================================================================================
    
    auto begin() const {
      return const_iterator::begin(this);
    }

    auto end() const {
      return const_iterator::end();
    }

    // =================================================================================================================
  };
}

#endif
