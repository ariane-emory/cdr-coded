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

    enum class tag : uintptr_t {
      //      last_element,
      link,
      element,
    };

    static constexpr
    char const * const
    tag_c_str(tag const & ct) {
      switch (ct) {
      default: return "ERROR";
#define CASE(enum_val) case enum_val: return # enum_val;        
        CASE(tag::element);
//        CASE(tag::last_element);
        CASE(tag::link);
#undef CASE
      }
    }

    static constexpr uchar_type FLAG_BITS_COUNT      { 2 };
    static constexpr uchar_type VALUE_BITS_COUNT     { (sizeof(value_type) << 3) - FLAG_BITS_COUNT };
    static constexpr value_type MASK_VALUE           { (1ul << VALUE_BITS_COUNT) - 1 };
    static constexpr value_type MASK_FLAG            { ~MASK_VALUE };
    static constexpr value_type FLAG_MASK_VALUE      { static_cast<value_type>(tag::element)      << VALUE_BITS_COUNT };
//    static constexpr value_type FLAG_MASK_LAST_VALUE { static_cast<value_type>(tag::last_element) << VALUE_BITS_COUNT };
    static constexpr value_type FLAG_MASK_LINK       { static_cast<value_type>(tag::link)         << VALUE_BITS_COUNT };

    value_type data;

    constexpr
    cell(value_type const & v, tag const & ct = tag::element) 
      : data(v | tag_to_flag_mask(ct)) {}

    constexpr
      cell(cell * const v, tag const & ct) 
      : data(std::bit_cast<value_type>(v) | tag_to_flag_mask(ct)) {}

    constexpr
    cell() 
      : data(0 | tag_to_flag_mask(tag::link)) {}
    
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
    
    DEFINE_LOGIC_ERROR_AND_ASSERT(cannot_be_a_link, tag::link != type())
    DEFINE_LOGIC_ERROR_AND_ASSERT(must_be_a_link,   tag::link == type())

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
      return data & MASK_FLAG;
    }

    constexpr
    tag type() const {
      return static_cast<tag>(flag() >> 62); 
    }

    constexpr
    bool is_type(tag const & ct) const {
      return type() == ct; 
    }

    constexpr
    cell const &
    operator*(){
      assert_must_be_a_link();
      return *link();
    }
    
    static
    void describe_class() {
      print("VALUE_BITS_COUNT:          ", VALUE_BITS_COUNT);
      print("FLAG_BITS_COUNT:           ", FLAG_BITS_COUNT);
      print_bits("MASK_FLAGS:                ", MASK_VALUE, false);
      print_bits("MASK_VALUE:                ", MASK_FLAG, false);
      print_bits("FLAG_MASK_VALUE:           ", FLAG_MASK_VALUE, false);
      //      print_bits("FLAG_MASK_LAST_VALUE:      ", FLAG_MASK_LAST_VALUE, false);
      print_bits("FLAG_MASK_LINK:            ", FLAG_MASK_LINK, false);
      putchar('\n');
    }
        
    void describe_instance() const {
      // printf("Cell at:                                                                                           %018p\n", this);
      print_bits("cell @                     ", reinterpret_cast<uintptr_t>(this));
      print_bits("cell.data:                 ", data);
      print_bits("cell.flag():               ", flag());
      print_bits("cell.type():               ", type());
      printf("cell.type() as c_str:       %s\n", tag_c_str(type()));
      if (is_type(tag::link))
        print_bits("cell.link():               ", reinterpret_cast<uintptr_t>(link()));
      else
        print_bits("cell.value()               ", value());
      putchar('\n');
    }
        
  private:
    static constexpr
    value_type
    tag_to_flag_mask(tag const & ct) {
      return static_cast<value_type>(ct) << VALUE_BITS_COUNT;
    }
    
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
        printf(" = 0x%016lx = % 24lu\n", tmp, tmp);
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
  //   reseune::cell::tag::link
  // };

  // static constinit
  // cell * nil = &_nil;

#ifdef RESEUNE_CONS
  struct cons {
    cell car, cdr;
    
    // constexpr
    // cons(cell::value_type const & car_, cell * const cdr_ = nil)
    //   : car(car_, cell::tag::element), cdr(cdr_, cell::tag::link) {}

    constexpr
    cons(cell::value_type const & car_, cell & cdr_)
      : car(car_, cell::tag::element), cdr(&cdr_, cell::tag::link) {}

    constexpr
    cons(cell::value_type const & car_, cons & cdr_)
      : car(car_, cell::tag::element), cdr(&cdr_.car, cell::tag::link) {}

    constexpr
    void prepend(cell::value_type const & new_car) {
      cdr = car;
      car = new_car;
    }
  };
#endif
}
#endif
