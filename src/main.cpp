#include <stdio.h>
#include <inttypes.h>
#include <stdexcept>

#include "reseune/reseune.hpp"

// =====================================================================================================================

#define LINE reseune::print_line()

constexpr size_t POOL_SIZE { 1<<8 }; // 256 cells, 8k memory

using cell = reseune::cell;
using tag  = cell::tag_type;
using link = reseune::link;
using pool = std::conditional<WITH_RESEUNE_POOL, reseune::mempool<cell, POOL_SIZE>, cell[POOL_SIZE]>::type;

constexpr pool POOL { 
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

// =====================================================================================================================

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

void describe_every_cell()   
{
  uint8_t ix { 0 };

  for (cell const & i : POOL) {
    printf("cell # %u\n", ix++);
    
    i.describe_instance();
  }

  LINE;
}

void describe_list(cell const & head) {
  uint8_t ix { 0 };
    
  for (cell const & c : head) {
    LINE;
    printf("cell #%u in this list: \n", ix++);
    c.describe_instance();
  }
}

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

// =====================================================================================================================

int main() {
  // describe_some_sizes();
  // cell::describe_class();
  // // describe_every_cell();
  // describe_list(POOL[0]); // list of 88s / Xs.
  // describe_list(POOL[4]); // list of 89s / Ys.
  // draw_the_pool();

  link l;
  link m;
  link n;
  link o;
  // l.describe_instance();
  // m.describe_instance();
  // n.describe_instance();
  // o.describe_instance();
  m.insert_before(l);
  n.insert_before(m);
  o.insert_before(n);
  l.describe_instance();
  m.describe_instance();
  n.describe_instance();
  o.describe_instance();
}



