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
