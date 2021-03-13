#include "flexpret_utils.h"
#include "flexpret_io.h"

void flexpret_error(const char* msg) {
    debug_string(msg);
}

void flexpret_info(const char* msg) {
    debug_string(msg);
}

void flexpret_not_implemented(const char f[])
{
    char buf[128] = "Function `";
    int func_len = strlen(f);
    strncpy(buf + 10, f, func_len);
    strncpy(buf + 10 + func_len, "` is not implemented.\n", func_len);
    flexpret_error(buf);
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

// required by gcc __func__ implementation
void *memset(void *s, int c, size_t n) {
    unsigned char *tmp = (unsigned char *)s;
    register int i;
    for (i = 0; i < n; i++) {
        tmp[i] = (unsigned char)c;
    }
    return s;
}
