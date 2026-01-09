#include "compiler.h"
#include "codegen.h"
#include "node.h"
#include "parser.h"
#include <stdlib.h>
#include <string.h>

static inline void scope_create(scope *scope, struct scope *parent) {
    scope->parent = parent;
    scope->extensions = NULL;
    scope->extension_count = 0;
}

static inline void scope_free(scope *scope) {
    for (int i = 0; i < scope->extension_count; i++)
        ext_free(&scope->extensions[i]);
    free(scope->extensions);
}

static inline void scope_back(scope **scope) {
    struct scope *parent = (*scope)->parent;
    scope_free(*scope);
    *scope = parent;
}

int compile(parser *parser, compiler *compiler) {
    compiler->parser = parser;
    compiler->ident = 0;
    compiler->error = NULL;
    scope_create(&compiler->global, NULL);
    compiler->scope = &compiler->global;
    compiler->result = malloc(1);
    if (compiler->result == NULL) {
        compiler->error = strdup("Error allocating memory");
        return -1;
    }
    compiler->result[0] = 0;
    return 0;
}

int compile_next(compiler *compiler) {
    node curr;
    int res = parse_next(compiler->parser, &curr);
    if (res < 0)
        return -1;
    if (res == 0)
        return 0;
    if (gen_html_node(compiler, &curr) < 0) {
        compiler->error = strdup("Error in code generation");
        return -1;
    }
    return 1;
}

char *compile_code(const char *code, int flags, char **error) {
    tokenizer tok;
    parser pars;
    compiler comp;
    if (tokenize(code, &tok) < 0 || parse(&tok, &pars) < 0 || compile(&pars, &comp) < 0) {
        *error = strdup("Error initializing compiler");
        return NULL;
    }
    if (flags & compile_ignore_comments)
        tok.nocomments = 1;
    if (gen_html_doctype(&comp) < 0) {
        *error = strdup("Error in code generation");
        compiler_free(&comp);
        parser_free(&pars);
        return NULL;
    }
    int res;
    while ((res = compile_next(&comp)) > 0);
    if (res < 0) {
        if (tok.error) {
            *error = tok.error;
            tok.error = NULL;
        } else if (pars.error) {
            *error = pars.error;
            pars.error = NULL;
        } else if (comp.error) {
            *error = comp.error;
            comp.error = NULL;
        }
        compiler_free(&comp);
        parser_free(&pars);
        tokenizer_free(&tok);
        return NULL;
    }
    char *result = comp.result;
    comp.result = NULL;
    compiler_free(&comp);
    parser_free(&pars);
    return result;
}

void compiler_free(compiler *compiler) {
    free(compiler->result);
    free(compiler->error);
    scope_free(&compiler->global);
}
