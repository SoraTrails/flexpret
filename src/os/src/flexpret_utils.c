#include "flexpret_utils.h"
#include "flexpret_io.h"
#include "flexpret_threads.h"
#include "flexpret_timing.h"

extern volatile hwthread_state startup_state[FLEXPRET_HW_THREADS_NUMS];
extern volatile hwtimer_state startup_timer_state[FLEXPRET_HW_THREADS_NUMS];

void flexpret_error(const char* msg) {
    debug_string(msg);
}

void flexpret_info(const char* msg) {
    debug_string(msg);
}

void flexpret_warn(const char* msg) {
    debug_string(msg);
}

void flexpret_not_implemented(const char f[])
{
    flexpret_error("Function `");
    flexpret_error(f);
    flexpret_error("` is not implemented.\n");
}

void flexpret_not_supported(const char f[])
{
    flexpret_error("Function `");
    flexpret_error(f);
    flexpret_error("` is not supported.\n");
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

void *memcpy(void *dst, const void *src, size_t len) {
    unsigned char * d = (unsigned char *) dst;
    const unsigned char * s = (const unsigned char *) src;
    register int i;
    for (i = 0; i < len; i++) {
        d[i] = s[i];
    }
    return dst;
}

// use to avoid generating mul instruction
uint32_t get_tid(osThreadId_t thread_id) {
    uint32_t offset = (uint32_t)thread_id - (uint32_t)startup_state;
    if (offset == 0) {
        return 0;
    } else if (offset == sizeof(hwthread_state)) {
        return 1;
    } else if (offset == sizeof(hwthread_state) << 1) {
        return 2;
    } else if (offset == ((sizeof(hwthread_state) << 1) + sizeof(hwthread_state))) {
        return 3;
    } else if (offset == sizeof(hwthread_state) << 2) {
        return 4;
    } else {
        flexpret_error("bad offset calling get_tid\n");
        return 0;
    }
}

uint32_t get_timer_tid(osTimerId_t timer_id) {
    uint32_t offset = (uint32_t)timer_id - (uint32_t)startup_timer_state;
    if (offset == 0) {
        return 0;
    } else if (offset == sizeof(hwtimer_state)) {
        return 1;
    } else if (offset == sizeof(hwtimer_state) << 1) {
        return 2;
    } else if (offset == ((sizeof(hwtimer_state) << 1) + sizeof(hwtimer_state))) {
        return 3;
    } else if (offset == sizeof(hwtimer_state) << 2) {
        return 4;
    } else {
        flexpret_error("bad offset calling get_timer_tid\n");
        return 0;
    }

}
