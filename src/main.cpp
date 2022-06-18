#include <iostream>
#include <string>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdexcept>
#include <tuple>
#include "reseune/reseune.hpp"

// ===============================================================================================================

#define LINE        (reseune::print_line())
#define HLINE       (reseune::print_line('-'))
#define NEWLINE     (putchar('\n'))
#define PRINT(x, y) (print_bits<true,false>(x, uintptr(y)))
#define cout        (std::cout)
#define endl        (std::endl)
#define uintptr(x)  (reseune::uintptr(x))

// ===============================================================================================================

constexpr size_t POOL_SIZE { 1<<8 }; // 256 cells, 8k memory

using namespace    reseune;

using string     = std::string;
using cell       = reseune::cell;
using tag        = cell::tag_type;
using link       = reseune::doubly_linked<char>;

using alloc_node = 
#ifdef RESEUNE_USE_ALLOC_INFO_WITH_UNFREE_FLAG
  alloc_info_with_unfree_flag
#else
  alloc_info
#endif
  ;

using pool       = std::conditional<
  WITH_RESEUNE_POOL,
  reseune::pool<cell, POOL_SIZE>,
  cell[POOL_SIZE]>::type;

#ifdef RESEUNE_SINGLETON_ALLOCATOR
#define ALLOC allocator<alloc_node>::
#else
#define ALLOC alloc. 
allocator<alloc_node> alloc {};  
#endif

#define malloc(s) ALLOC valloc(s, 1, verbose)
#define free(s) ALLOC release(s, verbose)

const bool verbose {false};

constexpr size_t buff_len = 1 << 14; // 16 kb

char buff1[buff_len] {0};
char buff2[buff_len] {0};

// ===============================================================================================================
::pool POOL { 
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

  NEWLINE;
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
    NEWLINE;
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

#define DESCRIBE(x) { x.describe_instance(); printf("Value               : %c\n", x.data); }
  DESCRIBE(l);
  DESCRIBE(m);
  DESCRIBE(n);
  DESCRIBE(o);

  NEWLINE;
  
  for (const link & i : l) DESCRIBE(i);
#undef DESCRIBE
}

// ===============================================================================================================
void setup_allocator() {
  // Give the allocator 2 blocks of memory =======================================================================
  ALLOC add_memory(buff1, buff_len, verbose);
  ALLOC add_memory(buff2, buff_len, verbose);
}

// ===============================================================================================================
void test_allocator() {
  // LINE;
  // PRINT("buff1 is at", buff1);
  // PRINT("buff2 is at", buff2);
  
  if (verbose) ALLOC describe_free_list();

  // Try allocating and  constructing a string ===================================================================
  {
    void * strblk = ALLOC valloc<string>(1, true);

    if (verbose) ALLOC describe_free_list();
    
    new (strblk) string("This is the string.");

    cout << *reinterpret_cast<string *>(strblk) << endl;

    PRINT("String is at", uintptr(strblk));
    NEWLINE;

    // Free the string ===========================================================================================

    ALLOC release(strblk, verbose, true);
  }

  printf("Before coalesce.\n");

  if (verbose) ALLOC describe_free_list();
  
  ALLOC coalesce(true);
  
  printf("After coalesce.\n");

  if (verbose) ALLOC describe_free_list();

  return;

  // Allocate ints until we run out of memory ====================================================================
  {
    using T = int;
  
    T *    buffer {nullptr};
    size_t ix     {0};

    do {
      if (verbose) {
        LINE;
        printf("Request #%zu, requesting %u bytes.\n", ++ix, sizeof(T) * 1024);
      }
    
      buffer = ALLOC alloc<T>(1024, verbose);
    
      if (verbose) {
        print_bits<verbose,false>("Received", uintptr(buffer));
        LINE;
        NEWLINE;

        if (verbose) ALLOC describe_free_list();
      }
    } while (nullptr != buffer);
  }
  
  printf("Before coalesce.\n");
  ALLOC coalesce(verbose);
  
  printf("After coalesce.\n");
  if (verbose) ALLOC describe_free_list();
}

// ===============================================================================================================
// describe_some_sizes();
// cell::describe_class();
// // describe_every_cell();
// describe_list(POOL[0]); // list of 88s / Xs.
// describe_list(POOL[4]); // list of 89s / Ys.
// draw_the_pool();
// test_links();
// measure_time(test_allocator);

// ===============================================================================================================
int main() {
  setup_allocator();

  // reseune::measure_time(test_allocator);
  
  const char * const sexp { "(((one two three four\n five six seven\n eight" };
  tokenizer          tok  { sexp };
  char *             word { nullptr };
  
  do {
    tok.discard_whitespace();

    // printf("fun @ %zu\n", &tokenizer::take_word);
    // printf("fun @ %zu\n", &tokenizer::take_one<is_char<'('>>);

    using t = tokenizer;
    
    tok.take_either<
      &t::take_word,
      &t::take_one<is_char<'('>>>();
    
    // word = tok.take_one(is_char<'('>);
    // if (nullptr == word)
    //   word = tok.take_word();
    
    if (nullptr == word) {
      printf("Word is null.\n");
    }
    else {
      printf("Word is '%s'.\n", word);
      free(word);
    }
  } while (nullptr != word);

  //ALLOC describe_free_list();

  tokenizer tok2 { "abc" };

  for (char c : tok2)
    putchar(c);

  putchar('\n');
}
