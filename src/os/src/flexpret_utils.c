#include "flexpret_utils.h"
#include "flexpret_io.h"

void flexpret_error(const char* msg) {
    debug_string(msg);
}

void flexpret_info(const char* msg) {
    debug_string(msg);
}

char * strncpy(char *dest, const char *src, size_t size) {
    if (dest == NULL)
        return NULL;
    char* ptr = dest;
    while (*src && size--)
    {
        *dest = *src;
        dest++;
        src++;
    }
    *dest = '\0';
    return ptr;
}


size_t strlen(const char *str) {
    const char *s;
    for (s = str; *s; ++s) 
        ;
    return (s - str);
}
