#include "compiler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *rdfile(const char *file) {
    FILE *f = fopen(file, "r");
    if (f == NULL)
        return NULL;
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
    const char *file = NULL;
    const char *output = NULL;
    int ignore_comments = 0;
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            if (argv[i][1] == '-') {
                if (strcmp(&argv[i][2], "output") == 0) {
                    if (i + 1 >= argc) {
                        fprintf(stderr, "Invalid argument: --output <file>\n");
                        return 1;
                    }
                    output = argv[i + 1];
                    i++;
                }
                if (strcmp(&argv[i][2], "no-comments") == 0)
                    ignore_comments = 1;
                continue;
            }
            for (int o = 1; argv[i][o] != 0; o++) {
                if (argv[i][o] == 'o') {
                    if (i + 1 >= argc) {
                        fprintf(stderr, "Invalid argument: --output <file>\n");
                        return 1;
                    }
                    output = argv[i + 1];
                    i++;
                    break;
                }
            }
            continue;
        }
        if (file) {
            fprintf(stderr, "Invalid positional argument: %s\n", argv[i]);
            return 1;
        }
        file = argv[i];
    }
    if (file == NULL) {
        fprintf(stderr, "Usage: chtml [-o <output file>] <file>\n");
        return 1;
    }
    char *code = rdfile(file);
    if (code == NULL) {
        printf("Error reading file\n");
        return 1;
    }
    char *error = NULL;
    char *res = compile_code(code, (ignore_comments ? compile_ignore_comments : 0), &error);
    free(code);
    if (res == NULL) {
        printf("%s\n", error);
        free(error);
        return 1;
    }
    if (output) {
        FILE *f = fopen(output, "w+");
        fwrite(res, strlen(res), 1, f);
        fclose(f);
    } else {
        printf("%s", res);
    }
    free(res);
    return 0;
}
