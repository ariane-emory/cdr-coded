#ifndef RESEUNE_ARRAY_FORWARD_ITERATOR
#define RESEUNE_ARRAY_FORWARD_ITERATOR

namespace reseune {
  template <typename TT>
  struct array_forward_iterator : public iterator_base<TT>
  {
    using value_type = TT;
    using base_type  = iterator_base<value_type>;
      
    constexpr array_forward_iterator(value_type * ptr): base_type(ptr) {}
      
    // prefix
    inline array_forward_iterator & operator ++ () {
      base_type::m_data++;

      return *this;
    }

    // postfix: untested 
    inline array_forward_iterator operator ++ (int) {
      array_forward_iterator tmp = *this;

      ++(*this);

      return tmp; 
    } 
  };

  // =============================================================================================================
}

#endif
