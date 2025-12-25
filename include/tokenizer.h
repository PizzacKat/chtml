#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "token.h"
typedef struct tokenizer {
    const char *str;
    const char *curr;
    int nocomments;
    char *error;
} tokenizer;

int tokenize(const char *str, tokenizer *tok);
int tokenize_next(tokenizer *tok, token *nxt);
char *tokenizer_error(tokenizer *tok);
void tokenizer_free(tokenizer *tok);

#endif
