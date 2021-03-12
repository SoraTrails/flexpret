#ifndef FLEXPRET_UTILS_H
#define FLEXPRET_UTILS_H
#include "flexpret_const.h"

#define FLEXPRET_ERROR(msg) \
    flexpret_error(msg); \
    
#define FLEXPRET_NOT_IMPLEMENTED(func, msg) \
    flexpret_error(#func##msg)

void flexpret_error(const char* msg);
void flexpret_info(const char* msg);

char * strncpy(char *dest, const char *src, size_t size);
size_t strlen(const char *s);


#endif