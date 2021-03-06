#ifndef RESEUNE_LINKED_LIST_FORWARD_CONST_ITERATOR_H
#define RESEUNE_LINKED_LIST_FORWARD_CONST_ITERATOR_H

#include "reseune/const_iterator_base/const_iterator_base.hpp"

namespace reseune {
  template <typename T>
  struct linked_list_forward_const_iterator : public const_iterator_base<T>
  {
    using base_type  = const_iterator_base<T>;
    using value_type = T;

    constexpr linked_list_forward_const_iterator(value_type const * ptr): base_type(ptr) {}

    inline auto operator ++ () { // prefix
      base_type::m_data = base_type::m_data->next;
      return *this;
    }
    
    inline auto operator ++ (int) { // postfix: untested
      linked_list_forward_const_iterator tmp = *this;
      base_type::m_data = base_type::m_data->next;
      return tmp;
    } 

    // friend auto operator == (const linked_list_forward_const_iterator & a, const linked_list_forward_const_iterator & b)
    //   { return a.base_type::m_data == b.base_type::m_data; };
    // friend auto operator != (const linked_list_forward_const_iterator & a, const linked_list_forward_const_iterator & b)
    //   { return a.base_type::m_data != b.base_type::m_data; };     

    inline static auto begin(value_type const * v) {
      return linked_list_forward_const_iterator { v };
    }

    inline static auto end() {
      return linked_list_forward_const_iterator { nullptr };
    }
  };    
}
#endif 
