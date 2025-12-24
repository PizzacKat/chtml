#ifndef PARSER_H
#define PARSER_H

#include "node.h"
#include "tokenizer.h"

typedef struct parser {
    tokenizer *tokenizer;
    int end;
    token curr;
} parser;

int parse(tokenizer *tokenizer, parser *parser);
int parse_next(parser *parser, node *nxt);
void parser_free(parser *parser);

#endif
