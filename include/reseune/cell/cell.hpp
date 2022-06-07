#ifndef RESEUNE_CELL_HPP
#define RESEUNE_CELL_HPP

#include <inttypes.h>
#include <stdio.h>
#include <bit>
#include <cassert>
#include <stdexcept>
#include "reseune/reseune.hpp"

namespace reseune {
  class cell {
  public:
    typedef uintptr_t value_type;
    typedef uint8_t uchar_type;

    static constexpr uchar_type TAG_BITS_COUNT      {
#ifdef RESEUNE_CELL_LAST_ELEMENT_OPTIMIZATION
      2
#else
      1
#endif
    };
    static constexpr uchar_type VALUE_BITS_COUNT    { (sizeof(value_type) << 3) - TAG_BITS_COUNT };
    static constexpr value_type MASK_VALUE          { (1ul << VALUE_BITS_COUNT) - 1 };
    static constexpr value_type MASK_TAG            { ~MASK_VALUE };

    enum class tag_t : uintptr_t {
#ifdef RESEUNE_CELL_LAST_ELEMENT_OPTIMIZATION
      last_element,
#endif
      link,
      element = 1ul << VALUE_BITS_COUNT,
    };

//     static constexpr value_type TAG_MASK_VALUE      { static_cast<value_type>(tag_t::element)      << VALUE_BITS_COUNT };
// #ifdef RESEUNE_CELL_LAST_ELEMENT_OPTIMIZATION
//     static constexpr value_type TAG_MASK_LAST_VALUE { static_cast<value_type>(tag_t::last_element) << VALUE_BITS_COUNT };
// #endif
//     static constexpr value_type TAG_MASK_LINK       { static_cast<value_type>(tag_t::link)         << VALUE_BITS_COUNT };

    value_type data;

    constexpr
    cell(value_type const & v, tag_t const & ct = tag_t::element
         ) 
      : data(v | static_cast<value_type>(ct)) {}

    constexpr
    cell(cell * const v, tag_t const & ct = tag_t::link) 
      : data(std::bit_cast<value_type>(v) | static_cast<value_type>(ct)) {}

    constexpr
    cell() 
      : data(0 | static_cast<value_type>(tag_t::link)) {}
    
#define DEFINE_LOGIC_ERROR_AND_ASSERT(name, expr)                               \
    class name : public std::logic_error {                                      \
    public:                                                                     \
    name() : std::logic_error(# name) {};                                       \
    };                                                                          \
    constexpr                                                                   \
    void                                                                        \
    assert_ ## name() const {                                                   \
      if constexpr(THROW) {                                                     \
        if (! (expr))                                                           \
          throw name {};                                                        \
      }                                                                         \
      else if (WARN) {                                                          \
        if (! (expr))                                                           \
          printf("WARNING: " # name  "\n");                                     \
      }                                                                         \
    }
    
    DEFINE_LOGIC_ERROR_AND_ASSERT(cannot_be_a_link, tag_t::link != type())
    DEFINE_LOGIC_ERROR_AND_ASSERT(must_be_a_link,   tag_t::link == type())

#undef DEFINE_LOGIC_ERROR_AND_ASSERT
    
    constexpr
    value_type value() const {
      assert_cannot_be_a_link();

      return get_value();
    }

    constexpr
    cell const *
    link() const {
      assert_must_be_a_link();
      
      return get_link();
    }
        
    constexpr
    value_type flag() const {
      return data & MASK_TAG;
    }

    constexpr
    tag_t type() const {
      return static_cast<tag_t>(flag()); 
    }

    constexpr
    bool is_type(tag_t const & ct) const {
      return type() == ct; 
    }

    constexpr
    bool is_element() const {
      return is_type(tag_t::element);
    }

    constexpr
    bool is_link() const {
      return is_type(tag_t::link);
    }

    constexpr
    cell const &
    operator*() const {
      assert_must_be_a_link();
      printf("DEREF!\n\n");
      return *link();
    }

    constexpr
    bool
    operator==(cell const & that) {
      return data == that.data;
    }

    constexpr
    bool
    operator!=(cell const & that) {
      return !operator==(that);
    }
    
    static constexpr
    char const * const
    tag_c_str(tag_t const & ct) {
      switch (ct) {
      default: return "ERROR";
#define CASE(enum_val) case enum_val: return # enum_val;
        CASE(tag_t::element);
#ifdef RESEUNE_CELL_LAST_ELEMENT_OPTIMIZATION
        CASE(tag_t::last_element);
#endif
        CASE(tag_t::link);
#undef CASE
      }
    }

    static
    void describe_class() {
      print("VALUE_BITS_COUNT:          ", VALUE_BITS_COUNT);
      print("TAG_BITS_COUNT:            ", TAG_BITS_COUNT);
      print_bits("MASK_TAG:                  ", MASK_VALUE, false);
      print_bits("MASK_VALUE:                ", MASK_TAG, false);
//       print_bits("TAG_MASK_VALUE:            ", TAG_MASK_VALUE, false);
// #ifdef RESEUNE_CELL_LAST_ELEMENT_OPTIMIZATION
//       print_bits("TAG_MASK_LAST_VALUE:       ", TAG_MASK_LAST_VALUE, false);
// #endif
//       print_bits("TAG_MASK_LINK:             ", TAG_MASK_LINK, false);
      putchar('\n');
    }
        
    void describe_instance() const {
      // printf("Cell at:                                                                                           %018p\n", this);
      print_bits("cell @                     ", reinterpret_cast<uintptr_t>(this));
      print_bits("cell.data:                 ", data);
      print_bits("cell.flag():               ", flag());
      print_bits("cell.type():               ", type());
      printf("cell.type() as c_str:       %s\n", tag_c_str(type()));
      if (is_type(tag_t::link))
        print_bits("cell.link():               ", reinterpret_cast<uintptr_t>(link()));
      else
        print_bits("cell.value()               ", value());
      putchar('\n');
    }
        
  private:
    // static constexpr
    // value_type
    // value_to_flag_mask(value_type const & vt) {
    //   return vt << VALUE_BITS_COUNT;
    // }

    // static constexpr
    // value_type
    // tag_to_flag_mask(tag_t const & ct) {
    //   return value_to_flag_mask(static_cast<value_type>(ct));
    // }
    
    constexpr
    value_type get_value() const {
      return data & MASK_VALUE;
    }

    constexpr
    cell const *
    get_link() const {
      return std::bit_cast<cell const *>(get_value());
    }

    template <typename T>
    static
    void print_bits(
      char const * descr, T const & v, bool const & print_int = true) {
      printf("%s 0b", descr);

      value_type tmp { static_cast<value_type>(v) };
            
      {
        uchar_type ix { 0 };
            
        for (value_type mask { 0b10000000'00000000'00000000'00000000'00000000'00000000'00000000'00000000ul };
             mask;
             mask >>= 1) {
          putchar((mask & tmp) ? '1' : '0');

          ++ix %= 8;

          if (0 == ix && mask > 1)
            putchar('\'');
        }
      }
            
      if (print_int)
        printf(" = 0x%016lx = % 20lu\n", tmp, tmp);
      else
        putchar('\n');
    }

    static
    void print(char const * descr, uchar_type const & v) {
      printf("%s %u\n", descr, v);
    }
  };

  static constinit
  cell nil {};
  // 0ul,
  //   reseune::cell::tag_t::link
  // };

  // static constinit
  // cell * nil = &_nil;

#ifdef RESEUNE_CONS
  struct cons {
    cell car, cdr;
    
    // constexpr
    // cons(cell::value_type const & car_, cell * const cdr_ = nil)
    //   : car(car_, cell::tag_t::element), cdr(cdr_, cell::tag_t::link) {}

    constexpr
    cons(cell::value_type const & car_, cell & cdr_)
      : car(car_, cell::tag_t::element), cdr(&cdr_, cell::tag_t::link) {}

    constexpr
    cons(cell::value_type const & car_, cons & cdr_)
      : car(car_, cell::tag_t::element), cdr(&cdr_.car, cell::tag_t::link) {}

    constexpr
    void prepend(cell::value_type const & new_car) {
      cdr = car;
      car = new_car;
    }
  };
#endif
}
#endif
