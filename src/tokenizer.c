#include "tokenizer.h"
#include <ctype.h>
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
        if (str == NULL)
            return -1;
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
        return 1;
    }
    if (curr(tok) == '"' && peek(tok) == '"' && peek2(tok) == '"') {
        next(tok);
        next(tok);
        const char *s = tok->curr + 1;
        while (next(tok) && (curr(tok) != '"' || peek(tok) != '"' || peek2(tok) != '"' || peek3(tok) == '"'));
        char *str = malloc(tok->curr - s + 1);
        if (str == NULL)
            return -1;
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
        while (next(tok) && curr(tok) != '"');
        char *str = malloc(tok->curr - s + 1);
        if (str == NULL)
            return -1;
        nxt->type = tok_txt;
        nxt->txt = strncpy(str, s, tok->curr - s);
        str[tok->curr - s] = 0;
        next(tok);
        return 1;
    }
    if (curr(tok) == '!') {
        while (next(tok) && isspace(curr(tok)));
        if (curr(tok) != '{')
            return -1;
        const char *s = tok->curr + 1;
        while (next(tok) && curr(tok) != '}');
        char *str = malloc(tok->curr - s + 1);
        if (str == NULL)
            return -1;
        nxt->type = tok_comment;
        nxt->txt = strncpy(str, s, tok->curr - s);
        str[tok->curr - s] = 0;
        next(tok);
        return 1;
    }

    return -1;
}
