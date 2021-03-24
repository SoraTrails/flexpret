#ifndef FLEXPRET_UTILS_H
#define FLEXPRET_UTILS_H
#include "flexpret_const.h"

#define FLEXPRET_ERROR(msg) \
    flexpret_error(msg); \
    
void flexpret_error(const char* msg);
void flexpret_info(const char* msg);
void flexpret_warn(const char* msg);
void flexpret_not_implemented(const char f[]);
void flexpret_not_supported(const char f[]);

char * strncpy(char *dest, const char *src, size_t size);
size_t strlen(const char *s);
void *memset(void *s, int c, size_t n);
void *memcpy(void *dst, const void *src, size_t len);
uint32_t get_tid_by_offset(uint32_t offset);

#endif