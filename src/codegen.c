#include "codegen.h"
#include "node.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static inline int scope_add(scope *scope, const ext *extension) {
    ext *extensions = realloc(scope->extensions, (scope->extension_count + 1) * sizeof(ext));
    if (extensions == NULL)
        return -1;
    scope->extensions = extensions;
    extensions[scope->extension_count] = *extension;
    ++scope->extension_count;
    return 0;
}

static inline int find_attr(const char *attrname, const attr *attrs, int count, const char **value) {
    for (int i = 0; i < count; i++) {
        if (strcmp(attrs[i].name, attrname) == 0) {
            *value = attrs[i].value;
            return 1;
        }
    }
    return 0;
}

static inline int cond_matches(const ext *e, const char *name, const attr *attrs, int count) {
    if (strcmp(e->name, name))
        return 0;
    int matches = 1;
    const char *value;
    for (int i = 0; i < e->cond_count; i++) {
        int has_value = find_attr(e->conds[i].name, attrs, count, &value);
        if (e->conds[i].type == cond_eq && 
            !(has_value &&
             (value == NULL && e->conds[i].value == NULL || value != NULL && e->conds[i].value != NULL && strcmp(value, e->conds[i].value) == 0))) {
            matches = 0;
            break;
        }
        if (e->conds[i].type == cond_neq &&
            !(!has_value ||
              ((value != NULL) != (e->conds[i].value != NULL)) ||
              (value != NULL && e->conds[i].value != NULL && strcmp(value, e->conds[i].value)))) {
            matches = 0;
            break;
        }
        if (e->conds[i].type == cond_in &&
            !(has_value &&
             (value && strstr(value, e->conds[i].value) != NULL))) {
            matches = 0;
            break;
        }
        if (e->conds[i].type == cond_nin && 
            !(!has_value ||
             (value && strstr(value, e->conds[i].value) == NULL))) {
            matches = 0;
            break;
        }
    }
    return matches;
}

static inline const ext *scope_find(scope *scope, const char *name, const attr *attrs, int count, const ext *curr) {
    int valid = 0;
    if (curr == NULL)
        valid = 1;
    while (scope) {
        for (int i = 0; i < scope->extension_count; i++) {
            const ext *e = &scope->extensions[i];
            if (e == curr) {
                valid = 1;
                continue;
            }
            if (cond_matches(e, name, attrs, count) && valid)
                return e;
        }
        scope = scope->parent;
    }
    return NULL;
}

int gen_insert(compiler *compiler, const char *str) {
    char *result = realloc(compiler->result, strlen(compiler->result) + strlen(str) + 1);
    if (result == NULL)
        return -1;
    compiler->result = result;
    strcat(compiler->result, str);
    return 0;
}

static inline int is_private_attr(const attr *attr) {
    return attr->name[0] == '_' && attr->name[1] == '_';
}

int gen_html_attr(compiler *compiler, const attr *attr) {
    size_t len = strlen(attr->name);
    if (attr->value)
        len += strlen(attr->value) + 3;
    char *str = malloc(len + 1);
    if (str == NULL)
        return -1;
    if (attr->value)
        sprintf(str, "%s=\"%s\"", attr->name, attr->value);
    else
        sprintf(str, "%s", attr->name);
    int res = gen_insert(compiler, str);
    free(str);
    return res;
}

int gen_html_attrs(compiler *compiler, const attr *attrs, int attr_count) {
    int c = 0;
    for (int i = 0; i < attr_count; i++) {
        if (is_private_attr(&attrs[i]))
            continue;
        if (c > 0)
            if (gen_insert(compiler, " ") < 0)
                return -1;
        c++;
        if (gen_html_attr(compiler, &attrs[i]) < 0)
            return -1;
    }
    return 0;
}

static inline int join_attrs(attr **attrs, int count, const attr *joining, int joining_count) {
    attr *tmp = realloc(*attrs, (count + joining_count) * sizeof(attr));
    if (tmp == NULL && count + joining_count != 0)
        return -1;
    memcpy(&tmp[count], joining, joining_count * sizeof(attr));
    *attrs = tmp;
    return count + joining_count;
}

static inline int count_non_private_attrs(const attr *attrs, int count) {
    int n = count;
    for (int i = 0; i < count; i++) {
        if (is_private_attr(&attrs[i]))
            n--;
    }
    return n;
}

static inline int join_nodes(node **nodes, int count, const node *joining, int joining_count) { 
    node *tmp = realloc(*nodes, (count + joining_count) * sizeof(node));
    if (tmp == NULL && count + joining_count != 0)
        return -1;
    memcpy(&tmp[count], joining, joining_count * sizeof(node));
    *nodes = tmp;
    return count + joining_count;
}

int gen_html_tag(compiler *compiler, const char *tag, const attr *attrs, int attr_count) {
    if (gen_insert(compiler, "<") < 0 || gen_insert(compiler, tag) < 0)
        return -1;
    const ext *e = NULL;
    if (count_non_private_attrs(attrs, attr_count) > 0)
        if (gen_insert(compiler, " ") < 0 || gen_html_attrs(compiler, attrs, attr_count) < 0)
            return -1;
    return gen_insert(compiler, ">");
}

int gen_html_closing_tag(compiler *compiler, const char *tag) {
    size_t len = strlen(tag) + 3;
    char *str = malloc(len + 1);
    if (str == NULL)
        return -1;
    sprintf(str, "</%s>", tag);
    int res = gen_insert(compiler, str);
    free(str);
    return res;
}

int gen_html_node(compiler *compiler, const node *node);

int gen_html_ident(compiler *compiler) {
    size_t len = compiler->ident * 4;
    char *str = malloc(len + 1);
    if (str == NULL)
        return -1;
    str[0] = 0;
    for (int i = 0; i < compiler->ident; i++)
        strcat(str, "    ");
    int res = gen_insert(compiler, str);
    free(str);
    return res;
}

scope *scope_create(scope *parent) {
    scope *scope = malloc(sizeof(struct scope));
    if (scope == NULL)
        return NULL;
    scope->parent = parent;
    scope->extensions = NULL;
    scope->extension_count = 0;
    return scope;
}

static inline void scope_free(scope *scope) {
    for (int i = 0; i < scope->extension_count; i++)
        ext_free(&scope->extensions[i]);
    free(scope->extensions);
    free(scope);
}

int gen_html_elem(compiler *compiler, const char *tag, const attr *attrs, int attr_count, const node *children, int count) {
    const ext *e = NULL;
    attr *new_attrs = NULL;
    int new_attr_count = join_attrs(&new_attrs, 0, attrs, attr_count);
    if (new_attr_count < 0) {
        compiler->error = strdup("Error creating element");
        return -1;
    }
    node *new_children = NULL;
    int new_count = 0;
    while ((e = scope_find(compiler->scope, tag, new_attrs, new_attr_count, e))) {
        if ((new_attr_count = join_attrs(&new_attrs, new_attr_count, e->attrs, e->attr_count)) < 0 || (new_count = join_nodes(&new_children, new_count, e->children, e->child_count)) < 0) {
            free(new_attrs);
            free(new_children);
            compiler->error = strdup("Error creating element");
            return -1;
        }
    }
    if (gen_html_tag(compiler, tag, new_attrs, new_attr_count) < 0) {
        free(new_attrs);
        free(new_children);
        return -1;
    }

    compiler->scope = scope_create(compiler->scope);

    if ((new_count = join_nodes(&new_children, new_count, children, count)) < 0) {
        free(new_attrs);
        free(new_children);
        compiler->error = strdup("Error creating element");
        return -1;
    }
 
    if (new_count > 0) {
        gen_insert(compiler, "\n");
        compiler->ident++;
        for (int i = 0; i < new_count; i++) {
            if (gen_html_node(compiler, &new_children[i]) < 0)
                return -1;
        }
        compiler->ident--;
        if (gen_html_ident(compiler) < 0)
            return -1;
    }
    free(new_attrs);
    free(new_children);
    scope *parent = compiler->scope->parent;
    scope_free(compiler->scope);
    compiler->scope = parent;
    return gen_html_closing_tag(compiler, tag);
}

#define STREQ(lstr, rstr) (strcmp(lstr, rstr) == 0)

int tag_is_html_void(const char *tag) {
    if (STREQ(tag, "area")) return 1;
    if (STREQ(tag, "base")) return 1;
    if (STREQ(tag, "br")) return 1;
    if (STREQ(tag, "col")) return 1;
    if (STREQ(tag, "embed")) return 1;
    if (STREQ(tag, "hr")) return 1;
    if (STREQ(tag, "img")) return 1;
    if (STREQ(tag, "input")) return 1;
    if (STREQ(tag, "link")) return 1;
    if (STREQ(tag, "meta")) return 1;
    if (STREQ(tag, "param")) return 1;
    if (STREQ(tag, "source")) return 1;
    if (STREQ(tag, "track")) return 1;
    if (STREQ(tag, "wbr")) return 1;
    return 0;
}

#undef STREQ

int gen_html_void(compiler *compiler, const char *tag, const attr *attrs, int attr_count) {
    const ext *e = NULL;
    attr *new_attrs = NULL;
    int new_attr_count = join_attrs(&new_attrs, 0, attrs, attr_count);
    if (new_attr_count < 0) {
        compiler->error = strdup("Error creating element");
        return -1;
    }
    node *new_children = NULL;
    int new_count = 0;
    while ((e = scope_find(compiler->scope, tag, new_attrs, new_attr_count, e))) {
        if ((new_attr_count = join_attrs(&new_attrs, new_attr_count, e->attrs, e->attr_count)) < 0 || (new_count = join_nodes(&new_children, new_count, e->children, e->child_count)) < 0) {
            free(new_attrs);
            free(new_children);
            compiler->error = strdup("Error creating element");
            return -1;
        }
    }
    if (gen_html_tag(compiler, tag, new_attrs, new_attr_count) < 0) {
        free(new_attrs);
        free(new_children);
        return -1;
    }

    if (tag_is_html_void(tag))
        return 0;

    compiler->scope = scope_create(compiler->scope);

    if (new_count > 0) {
        gen_insert(compiler, "\n");
        compiler->ident++;
        for (int i = 0; i < new_count; i++) {
            if (gen_html_node(compiler, &new_children[i]) < 0)
                return -1;
        }
        compiler->ident--;
        if (gen_html_ident(compiler) < 0)
            return -1;
    }
    free(new_attrs);
    free(new_children);
    scope *parent = compiler->scope->parent;
    scope_free(compiler->scope);
    compiler->scope = parent;
    return gen_html_closing_tag(compiler, tag);
}

int gen_html_txt(compiler *compiler, const char *txt) {
    return gen_insert(compiler, txt);
}

int gen_html_node(compiler *compiler, const node *node) {
    switch (node->type) {
        case node_elem:
            if (gen_html_ident(compiler) < 0)
                return -1;
            if (gen_html_elem(compiler, node->name, node->attrs, node->attr_count, node->children, node->child_count) < 0)
                return -1;
            return gen_insert(compiler, "\n");
        case node_void:
            if (gen_html_ident(compiler) < 0)
                return -1;
            if (gen_html_void(compiler, node->name, node->attrs, node->attr_count) < 0)
                return -1;
            return gen_insert(compiler, "\n");
        case node_txt:
            if (gen_html_ident(compiler) < 0)
                return -1;
            if (gen_html_txt(compiler, node->txt) < 0)
                return -1;
            return gen_insert(compiler, "\n");
        case node_comment:
            if (gen_html_ident(compiler) < 0)
                return -1;
            if (gen_html_comment(compiler, node->comment) < 0)
                return -1;
            return gen_insert(compiler, "\n");
        case node_ext:
            if (scope_add(compiler->scope, &node->ext) < 0)
                return -1;
            return 0;
    }
    return -1;
}

int gen_html_doctype(compiler *compiler) {
    return gen_insert(compiler, "<!DOCTYPE html>\n");
}

int gen_html_comment(compiler *compiler, const char *comment) {
    size_t len = strlen(comment) + 7;
    char *str = malloc(len + 1);
    if (str == NULL)
        return -1;
    sprintf(str, "<!--%s-->", comment);
    int res = gen_insert(compiler, str);
    free(str);
    return res;
}
