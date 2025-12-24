#ifndef NODE_H
#define NODE_H

typedef struct attr {
    char *name;
    char *value;
} attr;

typedef enum node_type {
    node_elem,
    node_void,
    node_txt,
    node_comment
} node_type;

typedef struct node {
    node_type type;
    union {
        char *txt;
        struct {
            char *name;
            attr *attrs;
            int attr_count;
            struct node *children;
            int child_count;
        };
        char *comment;
    };
} node;

void attr_free(attr *attr);
void node_free(node *node);

#endif
