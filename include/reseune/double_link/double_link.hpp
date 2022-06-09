#ifndef RESEUNE_DOUBLE_LINK_H
#define RESEUNE_DOUBLE_LINK_H

#include <cassert>

namespace reseune {
  class double_link {
  public:

    // =================================================================================================================
    // Typedefs
    // =================================================================================================================
    
    using pointer   = double_link *;
    using reference = double_link &;

    struct const_iterator;
    
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
    // const_iterator class
    // =================================================================================================================
    
    struct const_iterator
    {
      using value_type        = double_link;
      using difference_type   = std::ptrdiff_t;
      using iterator_category = std::input_iterator_tag;
      
      const_iterator(value_type const * ptr): m_value(ptr) {}
      
      value_type const & operator *  () const { return *m_value; }
      value_type const * operator -> () const { return m_value; }

      // prefix
      auto operator ++ () {
        m_value = m_value->next;
        return *this;
      }

      // postfix: untested 
      auto operator ++ (int) {
        const_iterator tmp = *this;
        m_value = m_value->next;
        return tmp;
      } 

      friend auto operator == (const const_iterator & a, const const_iterator & b) { return a.m_value == b.m_value; };
      friend auto operator != (const const_iterator & a, const const_iterator & b) { return a.m_value != b.m_value; };     

      static auto begin(value_type const * v) {
        return const_iterator { v };
      }

      static auto end() {
        return const_iterator { nullptr };
      }
      
    private:
      value_type const * m_value;
    };    

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
