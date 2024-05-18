#include <stdio.h>

#include <pico/binary_info.h>
#include <pico/stdlib.h>

#include <sys.h>

#define VERSION "v0.0"

int main() {
    bi_decl(bi_program_description("uhppoted-breakout"));
    bi_decl(bi_program_version_string(VERSION));

    stdio_init_all();
    setup_uart();

    printf(">>>>>>>>>>>>>>>>> wooooooot\n");
    blink();
}
