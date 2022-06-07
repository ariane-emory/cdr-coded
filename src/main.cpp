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
    
  // cell acell { 30, cell::tag::element };
  // cell bcell { &acell, cell::tag::link };

  cell::describe_class();

  // acell.describe_instance();
  // bcell.describe_instance();
  // (*bcell).describe_instance();
  
  cell cells[] = {
    /*  0 */ { 10, cell::tag::element },
    /*  1 */ { 20, cell::tag::element },
    /*  2 */ { 30, cell::tag::element },
    /*  3 */ { &cells[6], cell::tag::element },
    /*  4 */ nil, 
    /*  5 */ nil,
    /*  6 */ { 40, cell::tag::element },
    /*  7 */ { 50, cell::tag::element },
    /*  8 */ { 60, cell::tag::element },
    /*  9 */ { &cells[11], cell::tag::element },
    /* 10 */ nil,
    /* 11 */ { 70, cell::tag::element },
    /* 12 */ nil
  };

  for (cell & i : cells) {
    i.describe_instance();
  }
}






