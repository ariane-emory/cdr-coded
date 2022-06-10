#ifndef RESEUNE_CONST_ITERATOR_BASE_H
#define RESEUNE_CONST_ITERATOR_BASE_H

namespace reseune {
  template <typename T>
  struct const_iterator_base
  {
    using value_type        = T;
    using difference_type   = std::ptrdiff_t;
    using iterator_category = std::input_iterator_tag;

    constexpr const_iterator_base(value_type const * ptr): m_data(ptr) {}

    inline const value_type & operator *  () const { return *m_data; }
    inline const value_type * operator -> () const { return m_data; }

    // auto operator ++ () { // prefix
    //   m_data = m_data->next;
    //   return *this;
    // }
    
    // auto operator ++ (int) { // postfix: untested
    //   const_iterator_base tmp = *this;
    //   m_data = m_data->next;
    //   return tmp;
    // } 

    inline friend auto operator == (const const_iterator_base & a, const const_iterator_base & b)
      { return a.m_data == b.m_data; };

    inline friend auto operator != (const const_iterator_base & a, const const_iterator_base & b)
      { return a.m_data != b.m_data; };     

    // static auto begin(value_type const * v) {
    //   return const_iterator_base { v };
    // }

    // static auto end() {
    //   return const_iterator_base { nullptr };
    // }

  protected:
    value_type const * m_data;
  };    
}
#endif 
