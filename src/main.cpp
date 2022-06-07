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

int main() {
  describe_some_sizes();
    
  cell::describe_class();
  
  cell cells[] = {
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
    
    for (cell c : cells[0]) {
      LINE;
      printf("cell #%u\n", ix++);
      c.describe_instance();
    }
  }
}
