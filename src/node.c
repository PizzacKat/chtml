#include "node.h"
#include <stdlib.h>

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
            free(node->attrs);
            break;
        case node_txt:
            free(node->txt);
            break;
        case node_comment:
            free(node->comment);
            break;
    }
}
