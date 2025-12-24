#ifndef COMPILER_H
#define COMPILER_H

#include "parser.h"
typedef struct compiler {
    parser *parser;
    char *result;
    int ident;
} compiler;

int compile(parser *parser, compiler *compiler);
int compile_next(compiler *compiler);
void compiler_free(compiler *compiler);

char *compile_code(const char *code);

#endif
