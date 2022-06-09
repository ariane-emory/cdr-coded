#ifndef RESEUNE_MEMPOOL
#define RESEUNE_MEMPOOL

#define ALLOC_HEADER_SZ offsetof(alloc_node_t, block)

#ifndef align_up
#define align_up(num, align) (((num) + ((align)-1)) & ~((align)-1))
#endif

namespace reseune {
  template <typename T, size_t S>
  class mempool : public pool<T, S> {
    using base_type  = pool<T, S>;
    using value_type = base_type::value_type;

    struct ll_t {
      struct ll_t *next;
      struct ll_t *prev;
    };

    struct alloc_node_t {
      ll_t   node;
      size_t size;
      T *    block;
    };

    static inline void list_add_(ll_t * n, ll_t * prev, ll_t * next)
      {
        next->prev = n;
        n->next = next;
        n->prev = prev;
        prev->next = n;
      }


    static inline void list_add(ll_t * n, ll_t * head)
      {
        list_add_(n, head, head->next);
      }

    void malloc_addblock(void * addr, const size_t &size)
      {
        alloc_node_t *blk;

        // align the start addr of our block to the next pointer aligned addr
        blk = (void *) align_up((uintptr_t)addr, sizeof(void*));

        // calculate actual size - mgmt overhead
        blk->size = (uintptr_t) addr + size - (uintptr_t) blk 
          - ALLOC_HEADER_SZ;

        //and now our giant block of memory is added to the list!
        list_add(&blk->node, &base_type::data);
      }
  };
}

#undef ALLOC_HEADER_SZ

#endif
