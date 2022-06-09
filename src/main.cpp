#include <stdio.h>
#include <inttypes.h>
#include <stdexcept>

#include "reseune/cell/cell.hpp"
#include "reseune/pool/pool.hpp"
#include "reseune/mempool/mempool.hpp"

// =====================================================================================================================

#define LINE printf("=================================================================================================================================================\n\n")

constexpr size_t POOL_SIZE = 1<<8; // 256 cells, 8k memory

using cell = reseune::cell;
using tag  = cell::tag_type;
using pool =
#ifdef WITH_RESEUNE_POOL
  reseune::mempool<cell, POOL_SIZE>
#else
  cell[POOL_SIZE]
#endif
  ;

constexpr pool POOL= { 
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
  auto pool_size = POOL_SIZE*sizeof(cell);
  printf("POOL total size:            %zu bytes", pool_size);
  if (pool_size > 1<<10)
    printf(" (%zu kilobytes)", pool_size >> 10);
  putchar('\n');
  printf("sizeof(void *):             %zu\n", sizeof(void *));
  printf("sizeof(uintptr_t):          %zu\n", sizeof(uintptr_t));
  printf("sizeof(unsigned long):      %zu\n", sizeof(unsigned long));
  printf("sizeof(unsigned long long): %zu\n", sizeof(unsigned long long));
  printf("\n");
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
    printf("cell #%u\n", ix++);
    c.describe_instance();
  }
}

void draw_the_pool() {
  for (size_t ix = 0, line = 0; line < (POOL_SIZE >> 6); line++) {
    for (size_t col = 0; col < 64; col++, ix++)
    {
      // printf("%zu %zu %zu\n", ix, line, col);

      auto cell = POOL[ix];
        
      if (cell.is_nil()) {
        putchar('.');
      }
      else if (cell.is_element()) {
        putchar(static_cast<char>(cell.value()));
      }
#ifdef RESEUNE_CELL_LAST_ELEMENT_OPTIMIZATION
      else if (cell.is_last_element()) {
        putchar(static_cast<char>(cell.value() + 32));
      }
#endif
      else if (cell.is_link()) {
        if (cell.link() > &cell)
          putchar('<');
        else if (cell.link() < &cell)
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
  describe_some_sizes();
  cell::describe_class();
  // describe_every_cell();
  describe_list(POOL[0]); // list of 88s / Xs.
  describe_list(POOL[4]); // list of 89s / Ys.
  draw_the_pool();
}
