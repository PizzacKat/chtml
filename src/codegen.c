#include "codegen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int gen_insert(compiler *compiler, const char *str) {
    char *result = realloc(compiler->result, strlen(compiler->result) + strlen(str) + 1);
    if (result == NULL)
        return -1;
    compiler->result = result;
    strcat(compiler->result, str);
    return 0;
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
    for (int i = 0; i < attr_count; i++) {
        if (i > 0)
            if (gen_insert(compiler, " ") < 0)
                return -1;
        if (gen_html_attr(compiler, &attrs[i]) < 0)
            return -1;
    }
    return 0;
}

int gen_html_tag(compiler *compiler, const char *tag, const attr *attrs, int attr_count) {
    if (gen_insert(compiler, "<") < 0 || gen_insert(compiler, tag) < 0)
        return -1;
    if (attr_count > 0)
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

int gen_html_elem(compiler *compiler, const char *tag, const attr *attrs, int attr_count, const node *children, int count) {
    if (gen_html_tag(compiler, tag, attrs, attr_count) < 0)
        return -1;
    if (count > 0) {
        gen_insert(compiler, "\n");
        compiler->ident++;
        for (int i = 0; i < count; i++) {
            if (gen_html_node(compiler, &children[i]) < 0)
                return -1;
        }
        compiler->ident--;
        if (gen_html_ident(compiler) < 0)
            return -1;
    }
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
    if (gen_html_tag(compiler, tag, attrs, attr_count) < 0)
        return -1;
    if (tag_is_html_void(tag))
        return 0;
    return gen_html_closing_tag(compiler, tag);
}

int gen_html_txt(compiler *compiler, const char *txt) {
    return gen_insert(compiler, txt);
}

int gen_html_node(compiler *compiler, const node *node) {
    if (gen_html_ident(compiler) < 0)
        return -1;
    switch (node->type) {
        case node_elem:
            if (gen_html_elem(compiler, node->name, node->attrs, node->attr_count, node->children, node->child_count) < 0)
                return -1;
            return gen_insert(compiler, "\n");
        case node_void:
            if (gen_html_void(compiler, node->name, node->attrs, node->attr_count) < 0)
                return -1;
            return gen_insert(compiler, "\n");
        case node_txt:
            if (gen_html_txt(compiler, node->txt) < 0)
                return -1;
            return gen_insert(compiler, "\n");
        case node_comment:
            if (gen_html_comment(compiler, node->comment) < 0)
                return -1;
            return gen_insert(compiler, "\n");
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
