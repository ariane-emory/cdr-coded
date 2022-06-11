#ifndef RESEUNE_LINKED_LIST_FORWARD_ITERATOR_H
#define RESEUNE_LINKED_LIST_FORWARD_ITERATOR_H

#include "reseune/iterator_base/iterator_base.hpp"

namespace reseune {
  template <typename T>
  struct linked_list_forward_iterator : public iterator_base<T>
  {
    using base_type  = iterator_base<T>;
    using value_type = T;

    constexpr linked_list_forward_iterator(value_type * ptr): base_type(ptr) {}

    inline auto operator ++ () { // prefix
      base_type::m_data = base_type::m_data->next;
      return *this;
    }
    
    inline auto operator ++ (int) { // postfix: untested
      linked_list_forward_iterator tmp = *this;
      base_type::m_data = base_type::m_data->next;
      return tmp;
    } 

    // friend auto operator == (const linked_list_forward_iterator & a, const linked_list_forward_iterator & b)
    //   { return a.base_type::m_data == b.base_type::m_data; };
    // friend auto operator != (const linked_list_forward_iterator & a, const linked_list_forward_iterator & b)
    //   { return a.base_type::m_data != b.base_type::m_data; };     

    inline static auto begin(value_type * v) {
      return linked_list_forward_iterator { v };
    }

    inline static auto end() {
      return linked_list_forward_iterator { nullptr };
    }
  };    
}
#endif 
