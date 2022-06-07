#include <stdio.h>
#include <inttypes.h>
#include <stdexcept>

#include "reseune/cell/cell.hpp"

using namespace reseune;

void describe_some_sizes() {
  printf("sizeof(void *):             %zu\n", sizeof(void *));
  printf("sizeof(uintptr_t):          %zu\n", sizeof(uintptr_t));
  printf("sizeof(unsigned long):      %zu\n", sizeof(unsigned long));
  printf("sizeof(unsigned long long): %zu\n", sizeof(unsigned long long));
  printf("\n");
}

constexpr size_t MEMORY_SIZE = 1<<10; // 1024 cells, 8k memory

// cell cells[MEMORY_SIZE] = { 
//   /*  0 */ 10,
//   /*  1 */ 20,
//   /*  2 */ 30,
//   /*  3 */ &cells[6],
//   /*  4 */ nil, 
              //   /*  5 */ nil,
              //   /*  6 */ 40,
              //   /*  7 */ 50,
              //   /*  8 */ 60,
              //   /*  9 */ &cells[11],
              //   /* 10 */ nil,
              //   /* 11 */ 70,
              //   /* 12 */ nil
              // };

  cell cells[MEMORY_SIZE] = { 
    /*  0 */ 88,
    /*  1 */ 88,
    /*  2 */ 88,
    /*  3 */ &cells[7],
    /*  4 */ 89, 
    /*  5 */ &cells[13],
    /*  6 */ nullptr,
    /*  7 */ 88,
    /*  8 */ 88,
    /*  9 */ &cells[11],
    /* 10 */ nullptr,
    /* 11 */ { 88, cell::tag_t::last_element },
    /* 12 */ nullptr,
    /* 13 */ { 89, cell::tag_t::last_element },
    /* 14 */ 89,
    /* 15 */ &cells[17],
    /* 16 */ nullptr,
    /* 17 */ 89,
    /* 18 */ nullptr,
    /* 19 */ nullptr,
    /* 20 */ nullptr    
  };

int main() {
  describe_some_sizes();
    
  cell::describe_class();
  

#define LINE printf("=================================================================================================================================================\n\n")

if (false)
  {
    uint8_t ix { 0 };
  
    for (cell & i : cells) {
      printf("cell # %u\n", ix++);
    
      i.describe_instance();
    }

    LINE;
  }

if (true) {
  uint8_t ix { 0 };
    
  for (cell const & c : cells[0]) {
    LINE;
    printf("cell #%u\n", ix++);
    c.describe_instance();
  }
}

if (true) {
  uint8_t ix { 0 };
    
  for (cell const & c : cells[4]) {
    LINE;
    printf("cell #%u\n", ix++);
    c.describe_instance();
  }
}

if (true) {
  for (size_t ix = 0, line = 0; line < (MEMORY_SIZE >> 6); line++) {
    for (size_t col = 0; col < 64; col++, ix++)
    {
      // printf("%zu %zu %zu\n", ix, line, col);

      auto cell = cells[ix];
        
      if (cell.is_nil())
        putchar('.');
      else if (cell.is_element())
        putchar(static_cast<char>(cell.value()));
      else if (cell.is_last_element())
        putchar(static_cast<char>(cell.value() + 32));
      else if (cell.is_link()) {
        if (cell.link() > &cell)
          putchar('<');
        else if (cell.link() < &cell)
          putchar('>');
        else
          putchar('x');
      }
      else
        putchar('?');

      // else if (cell.is_nil())
      //   putchar('.');
      // else
      //   putchar('?');
    }
    putchar('\n');
  }
}
}
