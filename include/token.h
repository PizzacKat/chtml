#ifndef TOKEN_H
#define TOKEN_H

typedef enum token_type {
    tok_id,
    tok_lbrack,
    tok_rbrack,
    tok_lbrace,
    tok_rbrace,
    tok_txt,
    tok_eq,
    tok_comment
} token_type;

typedef struct token {
    token_type type;
    union {
        char *id;
        char *txt;
        char *comment;
    };
} token;

void token_free(token *token);

#endif
