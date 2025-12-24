#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "token.h"
typedef struct tokenizer {
    const char *str;
    const char *curr;
} tokenizer;

int tokenize(const char *str, tokenizer *tok);
int tokenize_next(tokenizer *tok, token *nxt);

#endif
