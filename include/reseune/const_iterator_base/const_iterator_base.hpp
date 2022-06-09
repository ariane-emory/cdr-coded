#ifndef RESEUNE_CONST_ITERATOR_BASE_H
#define RESEUNE_CONST_ITERATOR_BASE_H

namespace reseune {
  template <typename T>
  struct const_iterator_base
  {
    using value_type        = T;
    using difference_type   = std::ptrdiff_t;
    using iterator_category = std::input_iterator_tag;

    const_iterator_base(value_type const * ptr): m_value(ptr) {}

    const value_type & operator *  () const { return *m_value; }
    const value_type * operator -> () const { return m_value; }

    auto operator ++ () { // prefix
      m_value = m_value->next;
      return *this;
    }
    
    auto operator ++ (int) { // postfix: untested
      const_iterator_base tmp = *this;
      m_value = m_value->next;
      return tmp;
    } 

    friend auto operator == (const const_iterator_base & a, const const_iterator_base & b)
      { return a.m_value == b.m_value; };
    friend auto operator != (const const_iterator_base & a, const const_iterator_base & b)
      { return a.m_value != b.m_value; };     

    // static auto begin(value_type const * v) {
    //   return const_iterator_base { v };
    // }

    // static auto end() {
    //   return const_iterator_base { nullptr };
    // }

  protected:
    value_type const * m_value;
  };    
}
#endif 
