#ifndef RESEUNE_LINK_H
#define RESEUNE_LINK_H
#include <cassert>

namespace reseune {
  class link {
  public:

    // =================================================================================================================
    // Typedefs
    // =================================================================================================================
    
    using pointer   = link *;
    using reference = link &;

    struct const_iterator;
    
    // =================================================================================================================
    // Member fields
    // =================================================================================================================

    pointer next;

    // =================================================================================================================
    // Constructors
    // =================================================================================================================

    constexpr link(): next(nullptr) {}
    
    // =================================================================================================================
    // Printing-related member functions
    // =================================================================================================================
    
    inline void describe_instance() const {
      print_line();
      print_bits<true, false>("link is at", reinterpret_cast<uintptr_t>(this));
      print_bits<true, false>("next",       reinterpret_cast<uintptr_t>(next));
    }

    // =================================================================================================================
    // Other member functions
    // =================================================================================================================
    
    inline void insert_before(pointer next_) {
      next = next_;
    }
    inline void insert_before(reference next_) {
      insert_before(&next_);
    }

    // =================================================================================================================
    // const_iterator class
    // =================================================================================================================
    
    struct const_iterator
    {
      using value_type        = link;
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
