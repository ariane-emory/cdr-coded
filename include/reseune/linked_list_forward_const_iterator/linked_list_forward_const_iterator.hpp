#ifndef RESEUNE_LINKED_LIST_FORWARD_CONST_ITERATOR_H
#define RESEUNE_LINKED_LIST_FORWARD_CONST_ITERATOR_H

namespace reseune {
  template <typename T>
  struct linked_list_forward_const_iterator : public const_iterator_base<T>
  {
    using base_type         = const_iterator_base<T>;
    using value_type        = T;
    using difference_type   = std::ptrdiff_t;
    using iterator_category = std::input_iterator_tag;

    linked_list_forward_const_iterator(value_type const * ptr): base_type(ptr) {}

    const value_type & operator *  () const { return *base_type::m_value; }
    const value_type * operator -> () const { return base_type::m_value; }

    auto operator ++ () { // prefix
      base_type::m_value = base_type::m_value->next;
      return *this;
    }
    
    auto operator ++ (int) { // postfix: untested
      linked_list_forward_const_iterator tmp = *this;
      base_type::m_value = base_type::m_value->next;
      return tmp;
    } 

    friend auto operator == (const linked_list_forward_const_iterator & a, const linked_list_forward_const_iterator & b)
      { return a.base_type::m_value == b.base_type::m_value; };
    friend auto operator != (const linked_list_forward_const_iterator & a, const linked_list_forward_const_iterator & b)
      { return a.base_type::m_value != b.base_type::m_value; };     

    static auto begin(value_type const * v) {
      return linked_list_forward_const_iterator { v };
    }

    static auto end() {
      return linked_list_forward_const_iterator { nullptr };
    }
  };    
}
#endif 
