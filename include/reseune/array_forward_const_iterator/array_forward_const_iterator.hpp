#ifndef RESEUNE_ARRAY_FORWARD_CONST_ITERATOR
#define RESEUNE_ARRAY_FORWARD_CONST_ITERATOR

namespace reseune {
  template <typename TT>
  struct array_forward_const_iterator : public const_iterator_base<TT>
  {
    using value_type = TT;
    using base_type  = const_iterator_base<value_type>;
      
    constexpr array_forward_const_iterator(value_type const * ptr): base_type(ptr) {}
      
    // prefix
    inline array_forward_const_iterator & operator ++ () {
      base_type::m_data++;

      return *this;
    }

    // postfix: untested 
    inline array_forward_const_iterator operator ++ (int) {
      array_forward_const_iterator tmp = *this;

      ++(*this);

      return tmp; 
    } 
  };

  // ===================================================================================================================
}

#endif
