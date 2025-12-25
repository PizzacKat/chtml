#include "parser.h"
#include "node.h"
#include "token.h"
#include "tokenizer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    parser->error = NULL;
    if (next(parser) < 0)
        return -1;
    return 0;
}

int parse_attr(parser *parser, attr *attr) {
    attr->name = NULL;
    attr->value = NULL;
    if (curr(parser)->type != tok_id) {
        parser->error = strdup("Expected identifier for attribute");
        return -1;
    }
    attr->name = curr(parser)->id;
    if (next(parser) < 0)
        return -1;
    if (curr(parser)->type != tok_eq)
        return 0;
    if (next(parser) <= 0 || curr(parser)->type != tok_txt) {
        parser->error = strdup("Expected string for attribute");
        return -1;
    }
    attr->value = curr(parser)->txt;
    if (next(parser) < 0)
        return -1;
    return 0;
}

int parse_attrs(parser *parser, attr **attrs, int *count) {
    *attrs = NULL;
    *count = 0;
    if (curr(parser)->type != tok_lbrace || next(parser) <= 0) {
        parser->error = strdup("Expected '[' to start attribute list");
        return -1;
    }
    while (curr(parser) && curr(parser)->type != tok_rbrace) {
        attr *a = realloc(*attrs, (*count + 1) * sizeof(attr));
        if (a == NULL) {
            parser->error = strdup("Error allocating memory");
            return -1;
        }
        if (parse_attr(parser, &a[*count]) < 0) {
            attr_free(&a[*count]);
            return -1;
        }
        *attrs = a;
        (*count)++;
    }
    if (!curr(parser) || next(parser) < 0) {
        parser->error = strdup("Expected ']' to end attribute list");
        return -1;
    }
    return 0;
}

int parse_cond(parser *parser, cond *cond) {
    cond->type = cond_eq;
    cond->name = NULL;
    cond->value = NULL;
    if (curr(parser)->type != tok_id) {
        parser->error = strdup("Expected identifier for condition");
        return -1;
    }
    cond->name = curr(parser)->id;
    if (next(parser) < 0)
        return -1;
    token_type type = curr(parser)->type;
    if (type != tok_condeq && type != tok_condneq && type != tok_condin && type != tok_condnin)
        return 0;
    if (type == tok_condeq)
        cond->type = cond_eq;
    if (type == tok_condneq)
        cond->type = cond_neq;
    if (type == tok_condin)
        cond->type = cond_in;
    if (type == tok_condnin)
        cond->type = cond_nin;
    if (next(parser) <= 0 || curr(parser)->type != tok_txt) {
        parser->error = strdup("Expected string for condition");
        return -1;
    }
    cond->value = curr(parser)->txt;
    if (next(parser) < 0)
        return -1;
    return 0;
}

int parse_conds(parser *parser, cond **conds, int *count) {
    *conds = NULL;
    *count = 0;
    if (curr(parser)->type != tok_lbrace || next(parser) <= 0) {
        parser->error = strdup("Expected '[' to start condition list");
        return -1;
    }
    while (curr(parser) && (curr(parser)->type != tok_rbrace)) {
        cond *c = realloc(*conds, (*count + 1) * sizeof(cond));
        if (c == NULL) {
            parser->error = strdup("Error allocating memory");
            return -1;
        }
        if (parse_cond(parser, &c[*count]) < 0) {
            cond_free(&c[*count]);
            return -1;
        }
        *conds = c;
        (*count)++;
    }
    if (!curr(parser) || next(parser) < 0) {
        parser->error = strdup("Expected ']' to end condition list");
        return -1;
    }
    return 0;
}

int parse_conds_attrs(parser *parser, cond **conds, int *cond_count, attr **attrs, int *attr_count) {
    if (curr(parser)->type != tok_lbrace || next(parser) < 0) {
        parser->error = strdup("Expected '[' to start extension condition list");
        return -1;
    }
    while (curr(parser) && (curr(parser)->type != tok_rbrace && curr(parser)->type != tok_split)) {
        cond *c = realloc(*conds, (*cond_count + 1) * sizeof(cond));
        if (c == NULL) {
            parser->error = strdup("Error allocating memory");
            return -1;
        }
        if (parse_cond(parser, &c[*cond_count]) < 0) {
            cond_free(&c[*cond_count]);
            return -1;
        }
        *conds = c;
        (*cond_count)++;
    }
    if (curr(parser)->type == tok_rbrace) {
        if (next(parser) < 0)
            return -1;
        return 0;
    }
    if (next(parser) < 0)
        return -1; 
    while (curr(parser) && curr(parser)->type != tok_rbrace) {
        attr *a = realloc(*attrs, (*attr_count + 1) * sizeof(attr));
        if (a == NULL) {
            parser->error = strdup("Error allocating memory");
            return -1;
        }
        if (parse_attr(parser, &a[*attr_count]) < 0) {
            attr_free(&a[*attr_count]);
            return -1;
        }
        *attrs = a;
        (*attr_count)++;
    }
    if (!curr(parser) || next(parser) < 0) {
        parser->error = strdup("Expected ']' to end extension condition list");
        return -1;
    }
    return 0;
}

int parse_ext(parser *parser, ext *ext) {
    ext->name = NULL;
    ext->conds = NULL;
    ext->cond_count = 0;
    ext->attrs = NULL;
    ext->attr_count = 0;
    ext->children = NULL;
    ext->child_count = 0;
    if (curr(parser)->type != tok_ext || next(parser) < 0) {
        parser->error = strdup("Expected '#' for extension");
        return -1;
    }
    if (!curr(parser) || curr(parser)->type != tok_id) {
        parser->error = strdup("Expected identifier for extension");
        return -1;
    }
    ext->name = curr(parser)->id;
    if (next(parser) < 0)
        return -1;
    if (curr(parser)->type == tok_lbrace)
        if (parse_conds_attrs(parser, &ext->conds, &ext->cond_count, &ext->attrs, &ext->attr_count) < 0)
            return -1;
    if (curr(parser)->type == tok_lbrack) {
        if (next(parser) < 0)
            return -1;
        while (curr(parser) && curr(parser)->type != tok_rbrack) {
            node *children = realloc(ext->children, (ext->child_count + 1) * sizeof(node));
            if (children == NULL || parse_next(parser, &children[ext->child_count]) < 0)
                return -1;
            ext->children = children;
            ext->child_count++;
        }
        if (!curr(parser) || next(parser) < 0) {
            parser->error = strdup("Expected '}' to close extension's children");
            return -1;
        }
    }
    return 1;
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
        nxt->attr_count = 0;
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
            if (!curr(parser) || next(parser) < 0) {
                parser->error = strdup("Expected '}' to close element's children");
                return -1;
            }
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
    if (curr(parser)->type == tok_ext) {
        nxt->type = node_ext;
        return parse_ext(parser, &nxt->ext);
    }
    char *str = malloc(strlen("Expected node, got ") + strlen(token_type_to_string(curr(parser)->type)) + 1);
    if (str == NULL)
        return -1;
    sprintf(str, "Expected node, got %s", token_type_to_string(curr(parser)->type));
    parser->error = str;
    return -1;
}

void parser_free(parser *parser) {
    if (!parser->end)
        token_free(&parser->curr);
    free(parser->error);
}
