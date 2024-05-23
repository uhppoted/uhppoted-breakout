#include <stdio.h>
#include <string.h>

#include <PCAL6408APW.h>
#include <RX8900SA.h>

void exec(char *cmd) {
    char s[128];

    if (strncasecmp(cmd, "x", 1) == 0) {
        RX8900SA_init();
    }
}