#ifndef RESEUNE_TOKENIZER_LISPESQUE_TOKEN_TYPES_HPP
#define RESEUNE_TOKENIZER_LISPESQUE_TOKEN_TYPES_HPP

// =====================================================================================================================
// Lispesque token types
// =====================================================================================================================

#define TOKEN_TYPES                                                             \
  X(unlabeled)                                                                  \
  X(l_paren)                                                                    \
  X(r_paren)                                                                    \
  X(symbol)                                                                     \
  X(keyword)                                                                    \
  X(integer)                                                                    \
  X(quote)                                                                      \
  X(primitive)
    
namespace reseune {
#define X(name) name,
  enum lispesque_token_type {TOKEN_TYPES};
#undef X

#define X(name) #name,
  const char * const lispesque_token_type_strings[] {TOKEN_TYPES};
#undef X
}

// =====================================================================================================================
#undef TOKEN_TYPES
#endif
