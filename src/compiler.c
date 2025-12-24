#include "compiler.h"
#include "codegen.h"
#include "node.h"
#include "parser.h"
#include <stdlib.h>
#include <string.h>

int compile(parser *parser, compiler *compiler) {
    compiler->parser = parser;
    compiler->ident = 0;
    compiler->result = malloc(1);
    if (compiler->result == NULL)
        return -1;
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
    if (gen_html_node(compiler, &curr) < 0)
        return -1;
    return 1;
}

char *compile_code(const char *code) {
    tokenizer tok;
    parser pars;
    compiler comp;
    if (tokenize(code, &tok) < 0 || parse(&tok, &pars) < 0 || compile(&pars, &comp) < 0)
        return NULL;
    if (gen_html_doctype(&comp) < 0) {
        compiler_free(&comp);
        parser_free(&pars);
        return NULL;
    }
    int res;
    while ((res = compile_next(&comp)) > 0);
    if (res < 0) {
        compiler_free(&comp);
        parser_free(&pars);
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
}
