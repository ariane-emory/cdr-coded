#ifndef RESEUNE_ALLOCATOR_MACROS_HPP
#define RESEUNE_ALLOCATOR_MACROS_HPP

#define                    ASSERTISNOTNULL(x)   assert(ISNOTNULL(x))
#define                    BSIZE(b)             (b.data.size)
#define                    BSIZEP(pb)           (BSIZE((*pb)))
#define                    BSTART(b)            (&(b.data.block_start))
#define                    BSTARTP(pb)          (BSTART((*pb)))
#define                    CONS(head, list)     head.insert_before(list)
#define                    CONSP(headp, list)   CONS((*headp), list)
#define                    DESCRIBE(block)      { if (verbose) { block.describe_instance(); block.data.describe_instance(); } }
#define                    DESCRIBEP(pblock)    { DESCRIBE((*pblock)); }
#define                    DIE(...)             { WARN(__VA_ARGS__); assert(false); }
#define                    IFISNOTNULL(x)       if (ISNOTNULL(x))
#define                    IFISNULL(x)          if (ISNULL(x))
#define                    ISNOTNULL(x)         (nullptr != x)
#define                    ISNULL(x)            (nullptr == x)
#define                    PRINT(x, y)          { if (verbose) print_bits<true, false>((x), UINTPTR(y)); }
#define                    PRINTF(...)          { if (verbose) WARN(__VA_ARGS__); }
#define                    PUTCHAR(c)           { if (verbose) putchar(c); }
#define                    PVOIDFUN(name, ...)  INLINE PVOID name(__VA_ARGS__)
#define                    RCONS(tail, list)    tail.insert_after(list)
#define                    RCONSP(ptail, list)  RCONS((*ptail), list)
#define                    SETBSIZE(b, s)       b.data.size = s
#define                    SETBSIZEP(pb, s)     SETBSIZE((*pb), s)
#define                    UINTPTR(x)           (uintptr(x))
#define                    VOIDFUN(name, ...)   INLINE void name(__VA_ARGS__)
#define                    WARN(...)            { printf(__VA_ARGS__); }
#define                    ADDRARG              PVOIDC addr
#define                    ALLOC_HEADER_SZ      (offsetof(alloc_node, data.block_start))
#define                    FOR_EACH_BLOCK       for (auto & block : FREE_LIST_HEAD)
#define                    FREE_LIST_HEAD       (*PFREE_LIST_HEAD)
#ifdef RESEUNE_SINGLETON_ALLOCATOR
#define                    INLINE               static inline 
#else
#define                    INLINE               inline 
#endif
#define                    MIN_ALLOC_SZ         (ALLOC_HEADER_SZ + 32)
#define                    PROOT                (&root)
#define                    PFREE_LIST_HEAD      (root.next)
#define                    PRHLINE              { if (verbose) print_line('-'); }
#define                    PRLINE               { if (verbose) print_line(); }
#define                    PRNL                 PUTCHAR('\n')
#define                    PVOID                void *
#define                    PVOIDC               PVOID const 
#define                    SIZEARG              size_t size
#define                    VERBOSEARG           bool verbose = false

#define                    ALLOC_NODEP_TO_REF(name, x)                          \
  alloc_node & name {(* (reinterpret_cast<alloc_node *>(x)))}

#endif
