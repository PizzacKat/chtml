#ifndef NODE_H
#define NODE_H

typedef struct attr {
    char *name;
    char *value;
} attr;

typedef enum cond_type {
    cond_eq,
    cond_neq,
    cond_in,
    cond_nin
} cond_type;

typedef struct cond {
    cond_type type;
    char *name;
    char *value;
} cond;

typedef struct ext {
    char *name;
    cond *conds;
    int cond_count;
    attr *attrs;
    int attr_count;
    struct node *children;
    int child_count;
} ext;

typedef enum node_type {
    node_elem,
    node_void,
    node_txt,
    node_comment,
    node_ext
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
        ext ext;
    };
} node;

void ext_free(ext *ext);
void cond_free(cond *cond);
void attr_free(attr *attr);
void node_free(node *node);

#endif
