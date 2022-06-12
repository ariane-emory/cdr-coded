#ifndef RESEUNE_BASE_ONE_ALLOCATOR_MACROS_HPP
#define RESEUNE_BASE_ONE_ALLOCATOR_MACROS_HPP

#define                    ASSERTISNOTNULL(x)   assert(ISNOTNULL(x))
#define                    BSIZE(b)             (b.data.size)
#define                    BSIZEP(pb)           (BSIZE((*pb)))
#define                    BSTART(b)            (&b.data.block_start)
#define                    BSTARTP(pb)          (BSTART((*pb)))
#define                    CONS(head, list)     head.insert_before(list)
#define                    CONSP(headp, list)   CONS((*headp), list)
#define                    DESCRIBE(block)      { if (verbose) { block.describe_instance(); block.data.describe_instance(); } }
#define                    DESCRIBEP(pblock)    { DESCRIBE((*pblock)); }
#define                    IFISNOTNULL(x)       if (ISNOTNULL(x))
#define                    IFISNULL(x)          if (ISNULL(x))
#define                    ISNOTNULL(x)         (nullptr != x)
#define                    ISNULL(x)            (nullptr == x)
#define                    PALLOC_NODE(x)       (reinterpret_cast<palloc_node>(x))
#define                    PRINT(x, y)          { if (verbose) print_bits<true, false>((x), UINTPTR(y)); }
#define                    PRINTF(...)          { if (verbose) WARN(__VA_ARGS__); }
#define                    PUTCHAR(c)           { if (verbose) putchar(c); }

#ifdef RESEUNE_SINGLETON_ALLOCATOR
#define                    PVOIDFUN(name, ...)  static inline PVOID name(__VA_ARGS__)
#else
#define                    PVOIDFUN(name, ...)  inline PVOID name(__VA_ARGS__)
#endif

#define                    RCONS(tail, list)    tail.insert_after(list)
#define                    RCONSP(ptail, list)  RCONS((*ptail), list)
#define                    REMOVE(b)            b.remove()
#define                    REMOVEP(b)           REMOVE(*b)
#define                    SETBSIZE(b, s)       b.data.size = s
#define                    SETBSIZEP(pb, s)     SETBSIZE((*pb), s)
#define                    UINTPTR(x)           (uintptr(x))

#ifdef RESEUNE_SINGLETON_ALLOCATOR
#define                    VOIDFUN(name, ...)   static inline void name(__VA_ARGS__)
#else
#define                    VOIDFUN(name, ...)   inline void name(__VA_ARGS__)
#endif

#define                    WARN(...)            { printf(__VA_ARGS__); }
// This offset will only printg correctly for locations in MEMORY:
#define                    ADDRARG              PVOIDC addr
#define                    ALLOC_HEADER_SZ      (offsetof(alloc_node, data.block_start))
#define                    FOR_EACH_BLOCK       for (auto & block : FREE_LIST_HEAD)
#define                    FREE_LIST_HEAD       (*PFREE_LIST_HEAD)
#define                    MIN_ALLOC_SZ         (ALLOC_HEADER_SZ + 32)
#define                    PFREE_LIST           (&FREE_LIST)
#define                    PFREE_LIST_HEAD      (FREE_LIST.next)
#define                    PRHLINE              { if (verbose) print_line('-'); }
#define                    PRLINE               { if (verbose) print_line(); }
#define                    PVOID                void *
#define                    PVOIDC               PVOID const 
#define                    SIZEARG              size_t size
#define                    VERBOSEARG           bool verbose = false
#define                    palloc_node          alloc_node *

#define                    PLACE_BLOCKP(pblock)                                 \
  PRLINE;                                                                       \
  PRINT("Placing block", pblock);                                               \
                                                                                \
  palloc_node last_block {nullptr};                                             \
                                                                                \
  FOR_EACH_BLOCK                                                                \
  {                                                                             \
    last_block = &block;                                                        \
    PRINT("Compare with", &block);                                              \
    if (&block > pblock) {                                                      \
      PRINTF("PLACED BLOCK IS BEFORE THIS BLOCK.\n");                           \
      CONSP(pblock, block);                                                     \
      goto block_placed;                                                        \
    }                                                                           \
    else {                                                                      \
      PRINTF("PLACED BLOCK IS AFTER THIS BLOCK.\n");                            \
    }                                                                           \
  }                                                                             \
                                                                                \
  RCONSP(pblock, last_block);                                                   \
                                                                                \
block_placed:

#endif
