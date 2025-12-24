#include "parser.h"
#include "node.h"
#include "token.h"
#include "tokenizer.h"
#include <stdlib.h>

static inline token *curr(parser *parser) {
    if (parser->end)
        return NULL;
    return &parser->curr;
}

static inline int next(parser *parser) {
    int res = tokenize_next(parser->tokenizer, &parser->curr);
    if (res < 0)
        return -1;
    parser->end = !res;
    return res;
}

int parse(tokenizer *tokenizer, parser *parser) {
    parser->tokenizer = tokenizer;
    if (next(parser) < 0)
        return -1;
    return 0;
}

int parse_attr(parser *parser, attr *attr) {
    attr->name = NULL;
    attr->value = NULL;
    if (curr(parser)->type != tok_id)
        return -1;
    attr->name = curr(parser)->id;
    if (next(parser) < 0)
        return -1;
    if (curr(parser)->type != tok_eq)
        return 0;
    if (next(parser) <= 0 || curr(parser)->type != tok_txt)
        return -1;
    attr->value = curr(parser)->txt;
    if (next(parser) < 0)
        return -1;
    return 0;
}

int parse_attrs(parser *parser, attr **attrs, int *count) {
    *attrs = NULL;
    *count = 0;
    if (curr(parser)->type != tok_lbrace || next(parser) <= 0)
        return -1;
    while (curr(parser) && curr(parser)->type != tok_rbrace) {
        attr *a = realloc(*attrs, (*count + 1) * sizeof(attr));
        if (a == NULL)
            return -1;
        if (parse_attr(parser, &a[*count]) < 0) {
            attr_free(&a[*count]);
            return -1;
        }
        *attrs = a;
        (*count)++;
    }
    if (!curr(parser) || next(parser) < 0)
        return -1;
    return 0;
}

int parse_next(parser *parser, node *nxt) {
    if (!curr(parser))
        return 0;
    if (curr(parser)->type == tok_txt) {
        nxt->type = node_txt;
        nxt->txt = curr(parser)->txt;
        if (next(parser) < 0)
            return -1;
        return 1;
    }
    if (curr(parser)->type == tok_id) {
        nxt->type = node_void;
        nxt->name = curr(parser)->id;
        nxt->attrs = NULL;
        if (next(parser) < 0)
            return -1;
        if (curr(parser)->type == tok_lbrace)
            if (parse_attrs(parser, &nxt->attrs, &nxt->attr_count) < 0)
                return -1;
        if (curr(parser)->type == tok_lbrack) {
            nxt->type = node_elem;
            nxt->children = NULL;
            nxt->child_count = 0;
            if (next(parser) < 0)
                return -1;
            while (curr(parser) && curr(parser)->type != tok_rbrack) {
                node *children = realloc(nxt->children, (nxt->child_count + 1) * sizeof(node));
                if (children == NULL || parse_next(parser, &children[nxt->child_count]) < 0)
                    return -1;
                nxt->children = children;
                nxt->child_count++;
            }
            if (!curr(parser) || next(parser) < 0)
                return -1;
        }
        return 1;
    }
    if (curr(parser)->type == tok_comment) {
        nxt->type = node_comment;
        nxt->comment = curr(parser)->comment;
        if (next(parser) < 0)
            return -1;
        return 1;
    }
    return -1;
}

void parser_free(parser *parser) {
    if (!parser->end)
        token_free(&parser->curr);
}
