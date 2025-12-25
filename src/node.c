#include "node.h"
#include <stdlib.h>

void cond_free(cond *cond) {
    free(cond->name);
    free(cond->value);
}

void ext_free(ext *ext) {
    free(ext->name);
    for (int i = 0; i < ext->cond_count; i++)
        cond_free(&ext->conds[i]);
    free(ext->conds);
    for (int i = 0; i < ext->attr_count; i++)
        attr_free(&ext->attrs[i]);
    free(ext->attrs);
    for (int i = 0; i < ext->child_count; i++)
        node_free(&ext->children[i]);
    free(ext->children);
}

void attr_free(attr *attr) {
    free(attr->name);
    free(attr->value);
}

void node_free(node *node) {
    switch (node->type) {
        case node_void:
            for (int i = 0; i < node->child_count; i++)
                node_free(&node->children[i]);
            free(node->children);
        case node_elem:
            free(node->name);
            for (int i = 0; i < node->attr_count; i++)
                attr_free(&node->attrs[i]);
            free(node->attrs);
            break;
        case node_txt:
            free(node->txt);
            break;
        case node_comment:
            free(node->comment);
            break;
        case node_ext:
            ext_free(&node->ext);
            break;
    }
}
