#include "token.h"
#include <stdlib.h>

void token_free(token *token) {
    switch (token->type) {
        case tok_id:
            free(token->id);
            break;
        case tok_txt:
            free(token->txt);
            break;
        case tok_comment:
            free(token->comment);
            break;
        default:
            return;
    }
}

const char *token_type_to_string(token_type type) {
    switch (type) {
        case tok_id:
            return "<identifier>";
        case tok_lbrack:
            return "'{'";
        case tok_rbrack:
            return "'}'";
        case tok_lbrace:
            return "'['";
        case tok_rbrace:
            return "']'";
        case tok_txt:
            return "<string>";
        case tok_eq:
            return "'='";
        case tok_condeq:
            return "'=='";
        case tok_condneq:
            return "'!='";
        case tok_condin:
            return "'+='";
        case tok_condnin:
            return "'-='";
        case tok_comment:
            return "'!'";
        case tok_ext:
            return "'#'";
        case tok_split:
            return "'|'";
    }
}
