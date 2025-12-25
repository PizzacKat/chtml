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
    tok_condeq,
    tok_condneq,
    tok_condin,
    tok_condnin,
    tok_comment,
    tok_ext,
    tok_split
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
const char *token_type_to_string(token_type type);

#endif
