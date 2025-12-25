#include "tokenizer.h"
#include "token.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static inline char curr(tokenizer *tok) {
    return *tok->curr;
}

static inline char next(tokenizer *tok) {
    if (*tok->curr == 0)
        return 0;
    tok->curr++;
    return 1;
}

static inline char peek(tokenizer *tok) {
    if (*tok->curr == 0)
        return 0;
    return tok->curr[1];
}

static inline char peek2(tokenizer *tok) {
    if (*tok->curr == 0 || tok->curr[1] == 0)
        return 0;
    return tok->curr[2];
}

static inline char peek3(tokenizer *tok) {
    if (*tok->curr == 0 || tok->curr[1] == 0 || tok->curr[2] == 0)
        return 0;
    return tok->curr[3];
}

int tokenize(const char *str, tokenizer *tok) {
    if (str == NULL)
        return -1;
    tok->curr = tok->str = str;
    tok->nocomments = 0;
    tok->error = NULL;
    return 0;
}

int tokenize_next(tokenizer *tok, token *nxt) {
    while (isspace(curr(tok)) && next(tok));
    if (curr(tok) == 0)
        return 0;
    if (isalpha(curr(tok))) {
        const char *s = tok->curr;
        while (isalpha(curr(tok)) && next(tok));
        char *str = malloc(tok->curr - s + 1);
        if (str == NULL) {
            tok->error = strdup("Error allocating memory");
            return -1;
        }
        nxt->type = tok_id;
        nxt->id = strncpy(str, s, tok->curr - s);
        str[tok->curr - s] = 0;
        return 1;
    }
    if (curr(tok) == '{') {
        nxt->type = tok_lbrack;
        next(tok);
        return 1;
    }
    if (curr(tok) == '}') {
        nxt->type = tok_rbrack;
        next(tok);
        return 1;
    }
    if (curr(tok) == '[') {
        nxt->type = tok_lbrace;
        next(tok);
        return 1;
    }
    if (curr(tok) == ']') {
        nxt->type = tok_rbrace;
        next(tok);
        return 1;
    }
    if (curr(tok) == '=') {
        nxt->type = tok_eq;
        next(tok);
        if (curr(tok) == '=') {
            nxt->type = tok_condeq;
            next(tok);
        }
        return 1;
    }
    if (curr(tok) == '#') {
        nxt->type = tok_ext;
        next(tok);
        return 1;
    }
    if (curr(tok) == '|') {
        nxt->type = tok_split;
        next(tok);
        return 1;
    }
    if (curr(tok) == '"' && peek(tok) == '"' && peek2(tok) == '"') {
        next(tok);
        next(tok);
        const char *s = tok->curr + 1;
        while (next(tok) && (curr(tok) != '"' || peek(tok) != '"' || peek2(tok) != '"' || peek3(tok) == '"'));
        char *str = malloc(tok->curr - s + 1);
        if (str == NULL) {
            tok->error = strdup("Error allocating memory");
            return -1;
        }
        nxt->type = tok_txt;
        nxt->txt = strncpy(str, s, tok->curr - s);
        str[tok->curr - s] = 0;
        next(tok);
        next(tok);
        next(tok);
        return 1;
    }
    if (curr(tok) == '"') {
        const char *s = tok->curr + 1;
        while (next(tok) && curr(tok) != '"' && curr(tok) != '\n');
        if (curr(tok) != '"') {
            tok->error = strdup("Expected '\"' at end of string");
            return -1;
        }
        char *str = malloc(tok->curr - s + 1);
        if (str == NULL) {
            tok->error = strdup("Error allocating memory");
            return -1;
        }
        nxt->type = tok_txt;
        nxt->txt = strncpy(str, s, tok->curr - s);
        str[tok->curr - s] = 0;
        next(tok);
        return 1;
    }
    if (curr(tok) == '!') {
        next(tok);
        if (curr(tok) == '=') {
            nxt->type = tok_condneq;
            next(tok);
            return 1;
        }
        while (isspace(curr(tok)) && next(tok));
        if (curr(tok) != '{')
            return -1;
        const char *s = tok->curr + 1;
        while (next(tok) && curr(tok) != '}');
        if (tok->nocomments) {
            next(tok);
            return tokenize_next(tok, nxt);
        }
        char *str = malloc(tok->curr - s + 1);
        if (str == NULL) {
            tok->error = strdup("Error allocating memory");
            return -1;
        }
        nxt->type = tok_comment;
        nxt->txt = strncpy(str, s, tok->curr - s);
        str[tok->curr - s] = 0;
        next(tok);
        return 1;
    }
    if (curr(tok) == '+' && peek(tok) == '=') {
        next(tok);
        next(tok);
        nxt->type = tok_condin;
        return 1;
    }
    if (curr(tok) == '-' && peek(tok) == '=') {
        next(tok);
        next(tok);
        nxt->type = tok_condnin;
        return 1;
    }
    
    char *error = malloc(strlen("Invalid character '?'") + 1);
    if (error == NULL)
        return -1;
    sprintf(error, "Invalid character '%c'", curr(tok));
    tok->error = error;
    return -1;
}

char *tokenizer_error(tokenizer *tok) {
    return tok->error;
}

void tokenizer_free(tokenizer *tok) {
    free(tok->error);
}
