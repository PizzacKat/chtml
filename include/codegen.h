#ifndef CODEGEN_H
#define CODEGEN_H

#include "compiler.h"

int gen_insert(compiler *compiler, const char *str);
int gen_html_ident(compiler *compiler);
int gen_html_doctype(compiler *compiler);
int gen_html_comment(compiler *compiler, const char *comment);
int gen_html_node(compiler *compiler, const node *node);
int gen_html_txt(compiler *compiler, const char *txt);
int gen_html_void(compiler *compiler, const char *tag, const attr *attrs, int attr_count);
int gen_html_elem(compiler *compiler, const char *tag, const attr  *attrs, int attr_count, const node *children, int count);
int gen_html_tag(compiler *compiler, const char *tag, const attr *attrs, int attr_count);
int gen_html_closing_tag(compiler *compiler, const char *tag);
int gen_html_attrs(compiler *compiler, const attr *attrs, int attr_count);
int gen_html_attr(compiler *compiler, const attr *attr);

#endif
