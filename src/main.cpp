#include <stdio.h>
#include <inttypes.h>
#include <stdexcept>

#include "reseune/cell/cell.hpp"
#include "reseune/pool/pool.hpp"

using namespace reseune;

constexpr size_t POOL_SIZE = 1<<10; // 1024 cells, 8k memory

#ifdef WITH_RESEUNE_POOL
constexpr pool<cell, POOL_SIZE> POOL
#else
constexpr cell POOL[POOL_SIZE]
#endif
= { 
  /*  0 */ 88,
  /*  1 */ 88,
  /*  2 */ 88,
  /*  3 */ &POOL[6],
  /*  4 */ 89, 
  /*  5 */ &POOL[12],
  /*  6 */ 88,
  /*  7 */ 88,
  /*  8 */ 88,
  /*  9 */ &POOL[11],
  /* 10 */ nullptr,
  /* 11 */ { 88, cell::tag_t::last_element },
  /* 12 */ 89,
  /* 13 */ 89,
  /* 14 */ 89,
  /* 15 */ &POOL[17],
  /* 16 */ nullptr,
  /* 17 */ { 89, cell::tag_t::last_element },
  /* 18 */ nullptr,
  /* 19 */ nullptr,
  /* 20 */ nullptr    
};

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

int main() {
  describe_some_sizes();
    
  cell::describe_class();
  
  if (false)
  {
    uint8_t ix { 0 };

#ifdef WITH_RESEUNE_POOL
#define DATA (reseune.pool)
#else
#define DATA (POOL)
#endif
    for (cell const & i : POOL.data) {
      printf("cell # %u\n", ix++);
    
      i.describe_instance();
    }


#define LINE printf("=================================================================================================================================================\n\n")
    LINE;
  }

  if (true) {
    uint8_t ix { 0 };
    
    for (cell const & c : POOL[0]) {
      LINE;
      printf("cell #%u\n", ix++);
      c.describe_instance();
    }
  }

  if (true) {
    uint8_t ix { 0 };
    
    for (cell const & c : POOL[4]) {
      LINE;
      printf("cell #%u\n", ix++);
      c.describe_instance();
    }
  }

  if (true) {
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
        else if (cell.is_last_element()) {
          putchar(static_cast<char>(cell.value() + 32));
        }
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

        // else if (cell.is_nil())
        //   putchar('.');
        // else
        //   putchar('?');
      }
      putchar('\n');
    }
  }
}
