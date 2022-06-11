#include <stdio.h>
#include <inttypes.h>
#include <stdexcept>

#include "reseune/reseune.hpp"

// ===============================================================================================================

#define LINE reseune::print_line()

constexpr size_t POOL_SIZE { 1<<8 }; // 256 cells, 8k memory

using cell = reseune::cell;
using tag  = cell::tag_type;
using link = reseune::doubly_linked<char>;
using pool = std::conditional<WITH_RESEUNE_POOL, reseune::pool<cell, POOL_SIZE>, cell[POOL_SIZE]>::type;

pool POOL { 
  /*  0 */ 88,
  /*  1 */ 88,
  /*  2 */ 88,
  /*  3 */ &POOL[6],
  /*  4 */ 89, 
  /*  5 */ &POOL[13],
  /*  6 */ 88,
  /*  7 */ 88,
  /*  8 */ 88,
  /*  9 */ &POOL[10],
  /* 10 */ &POOL[11],
#ifdef RESEUNE_CELL_LAST_ELEMENT_OPTIMIZATION
  /* 11 */ { 88, tag::last_element },
#else
  /* 11 */ 88,
#endif
  /* 12 */ nullptr,
  /* 13 */ 89,
  /* 14 */ 89,
  /* 15 */ &POOL[17],
  /* 16 */ nullptr,
#ifdef RESEUNE_CELL_LAST_ELEMENT_OPTIMIZATION
  /* 17 */ { 89, tag::last_element },
#else
  /* 17 */ 89,
#endif
  /* 18 */ nullptr,
  /* 19 */ nullptr,
  /* 20 */ nullptr    
};

// ===============================================================================================================

void describe_some_sizes() {
  LINE;
  
  auto pool_size { POOL_SIZE*sizeof(cell) };

  printf("POOL total size:            %zu bytes", pool_size);

  if (pool_size > 1<<10)
    printf(" (%zu kilobytes)", pool_size >> 10);

  putchar('\n');
  printf("sizeof(void *):             %zu\n", sizeof(void *));
  printf("sizeof(uintptr_t):          %zu\n", sizeof(uintptr_t));
  printf("sizeof(unsigned long):      %zu\n", sizeof(unsigned long));
  printf("sizeof(unsigned long long): %zu\n", sizeof(unsigned long long));

  LINE;
}

// ===============================================================================================================

void describe_every_cell()   
{
  uint8_t ix { 0 };

  for (const auto & i : POOL) {
    printf("cell # %u\n", ix++);
    
    i.describe_instance();
  }

  LINE;
}

// ===============================================================================================================

void describe_list(const cell & head) {
  uint8_t ix { 0 };
    
  for (const auto & c : head) {
    LINE;
    printf("cell #%u in this list: \n", ix++);
    c.describe_instance();
  }
}

// ===============================================================================================================

void draw_the_pool() {
  LINE;
    
  for (size_t ix { 0 }, line { 0 }; line < (POOL_SIZE >> 6); line++) {
    for (size_t col { 0 }; col < 64; col++, ix++)
    {
      // printf("%zu %zu %zu\n", ix, line, col);

      auto c { POOL[ix] };
        
      if (c.is_nil()) {
        putchar('.');
      }
      else if (c.is_element()) {
        putchar(static_cast<char>(c.value()));
      }
#ifdef RESEUNE_CELL_LAST_ELEMENT_OPTIMIZATION
      else if (c.is_last_element()) {
        putchar(static_cast<char>(c.value() + 32));
      }
#endif
      else if (c.is_link()) {
        if (c.link() > &c)
          putchar('<');
        else if (c.link() < &c)
          putchar('>');
        else
          putchar('x');
      }
      else {
        putchar('?');
      }
    }
    putchar('\n');
  }
}

// ===============================================================================================================

void test_links() {
  link l; l.data = 'l';
  link m; m.data = 'm';
  link n; n.data = 'n';
  link o; o.data = 'o';
  // l.describe_instance();
  // m.describe_instance();
  // n.describe_instance();
  // o.describe_instance();
  m.insert_after(l);
  n.insert_after(m);
  o.insert_after(n);

  m.remove();

#define PRINT(x) { x.describe_instance(); printf("Value               : %c\n", x.data); }

  PRINT(l);
  PRINT(m);
  PRINT(n);
  PRINT(o);

  putchar('\n');
  
  for (const link & i : l) PRINT(i);
  
#undef PRINT
}

// ===============================================================================================================

void test_base_one() {
  using namespace reseune::base_one;
  
  alloc_add_block(MEMORY, MEMORY_BYTES);

  // This size is probably wrong by 8:
  // reseune::print_bits<true, false>("Size should be", MEMORY_BYTES - sizeof(alloc_node)); 
  // putchar('\n');
  // LINE;
  
  // reseune::print_bits<true,false>("Free list is now at", reseune::uintptr(&reseune::base_one::FREE_LIST));

  describe_free_list();
  
  alloc(1024);

  describe_free_list();
}

// ===============================================================================================================

int main() {
  // describe_some_sizes();
  // cell::describe_class();
  // // describe_every_cell();
  // describe_list(POOL[0]); // list of 88s / Xs.
  // describe_list(POOL[4]); // list of 89s / Ys.
  // draw_the_pool();
  // test_links();
  test_base_one();
}

