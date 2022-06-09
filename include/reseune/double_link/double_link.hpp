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
      print_bits<true, false>("next",       reinterpret_cast<uintptr_t>(next));
      print_bits<true, false>("prev",       reinterpret_cast<uintptr_t>(prev));
    }

    // =================================================================================================================
    // Other member functions
    // =================================================================================================================
    
    // inline void connect(pointer next_, pointer prev_) {
    //   if (nullptr != next) next_->prev = this;
    //   next        = next_;
    //   prev        = prev_;
    //   if (nullptr != prev) prev->next  = this;      
    // }
    // inline void connect(pointer head) {
    //   connect(head, head->next);
    // }
                        
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
