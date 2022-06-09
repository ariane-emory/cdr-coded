#ifndef RESEUNE_ALLOC_BLOCK_H
#define RESEUNE_ALLOC_BLOCK_H

namespace reseune {
  public alloc_block {
  public:
    link   node;
    size_t size;
    char * block;
  };
}

#endif
