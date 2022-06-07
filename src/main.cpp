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

cell cells[1 << 12] = { // 4096 cells, 32k memory
  /*  0 */ 10,
  /*  1 */ 20,
  /*  2 */ 30,
  /*  3 */ &cells[6],
  /*  4 */ nil, 
  /*  5 */ nil,
  /*  6 */ 40,
  /*  7 */ 50,
  /*  8 */ 60,
  /*  9 */ &cells[11],
  /* 10 */ nil,
  /* 11 */ 70,
  /* 12 */ nil
};

int main() {
  describe_some_sizes();
    
  cell::describe_class();
  

#define LINE printf("=================================================================================================================================================\n")

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
    for (size_t ix = 0, line = 0; line < 32; line++) {
      for (size_t col = 0; col < 128; col++, ix++)
      {
        // printf("%zu %zu %zu\n", ix, line, col);

        auto cell = cells[ix];
        
        if (cell.is_element())
          putchar('e');
        else if (cell.is_nil())
          putchar('.');
        else if (cell.is_link())
          putchar('l');
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
