#include "cmsis_os2.h"
#include "flexpret_threads.h"
#include "flexpret_io.h"

int main() {
    char stack0[128];
    char stack1[128];
    char stack2[128];
    debug_string(itoa_hex((uint32_t) stack0));
    debug_string("\n");
    debug_string(itoa_hex((uint32_t) stack1));
    debug_string("\n");
    debug_string(itoa_hex((uint32_t) stack2));
    debug_string("\n");
    return 0;
}