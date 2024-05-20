#include <stdio.h>
#include <string.h>

void exec(char *cmd) {
    char s[128];

    snprintf(s, sizeof(s), "%s", cmd);
    printf("\n---- %s\n", s);
}