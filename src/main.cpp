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
    
  cell acell { 33, cell::cell_type::element };
  cell bcell { &acell, cell::cell_type::rest };

  cell::describe_class();
  acell.describe_instance();
  bcell.describe_instance();
  (*bcell).describe_instance();


// bcell.value();
  // bcell.rest();
}






