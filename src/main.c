#include "compiler.h"
#include <stdio.h>
#include <stdlib.h>

char *rdfile(const char *file) {
    FILE *f = fopen(file, "r");
    fseek(f, 0, SEEK_END);
    size_t sz = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *data = malloc(sz + 1);
    fread(data, sz, 1, f);
    fclose(f);

    data[sz] = 0;

    return data;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: chtml <file>\n");
        return 1;
    }
    char *code = rdfile(argv[1]);
    char *res = compile_code(code);
    printf("%s", res);
    free(code);
    free(res);
    return 0;
}
