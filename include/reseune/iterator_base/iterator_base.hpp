#ifndef RESEUNE_ITERATOR_BASE_H
#define RESEUNE_ITERATOR_BASE_H

#include <iterator>

namespace reseune {
  template <typename T>
  class iterator_base
  {
  public:
    using value_type        = T;
    using difference_type   = std::ptrdiff_t;
    using iterator_category = std::input_iterator_tag;

    constexpr iterator_base(value_type * ptr): m_data(ptr) {}

    virtual inline value_type & operator *  () const { return *m_data; }
    inline  value_type * operator -> () const { return m_data; }

    // auto operator ++ () { // prefix
    //   m_data = m_data->next;
    //   return *this;
    // }
    
    // auto operator ++ (int) { // postfix: untested
    //   iterator_base tmp = *this;
    //   m_data = m_data->next;
    //   return tmp;
    // } 

    inline friend auto operator == (const iterator_base & a, const iterator_base & b)
      { return a.m_data == b.m_data; };

    inline friend auto operator != (const iterator_base & a, const iterator_base & b)
      { return a.m_data != b.m_data; };     

    // static auto begin(value_type const * v) {
    //   return iterator_base { v };
    // }

    // static auto end() {
    //   return iterator_base { nullptr };
    // }

  protected:
    value_type * m_data;
  };    
}
#endif 
