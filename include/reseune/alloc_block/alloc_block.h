#ifndef RESEUNE_ALLOC_BLOCK_H
#define RESEUNE_ALLOC_BLOCK_H

#define ALLOC_HEADER_SZ offsetof(alloc_node_t, block)

namespace reseune {
  public alloc_block {
  public:
    link   node;
    size_t size;
    char * block;

    alloc_block(): node({}), size(0), block(nullptr);
  };
}

#endif
