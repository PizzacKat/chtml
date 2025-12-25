#ifndef COMPILER_H
#define COMPILER_H

#include "parser.h"
typedef struct compiler {
    parser *parser;
    char *result;
    int ident;
    char *error;
    ext *extensions;
    int extension_count;
} compiler;

int compile(parser *parser, compiler *compiler);
int compile_next(compiler *compiler);
void compiler_free(compiler *compiler);

typedef enum compile_flags {
    compile_ignore_comments = 0x01
} compile_flags;

char *compile_code(const char *code, int flags, char **error);

#endif
