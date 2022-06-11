#include "reseune/base_one/base_one.hpp"

reseune::base_one::alloc_node reseune::base_one::FREE_LIST {nullptr, nullptr};
char * reseune::base_one::MEMORY[reseune::base_one::MEMORY_BYTES] = { 0 };
