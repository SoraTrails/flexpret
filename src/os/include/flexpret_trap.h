#ifndef FLEXPRET_TRAP_H
#define FLEXPRET_TRAP_H
#include "cmsis_os2.h"
#include "flexpret_kernel.h"

typedef void (*trap_handler)();

typedef struct trap_vector {
    trap_handler handler[4];
} trap_vector;

typedef enum  {
    InterruptOnExpire = 0,
    ExceptionOnExpire = 1,
    ExternalInterrupt = 2,
    Exception         = 3,
    TRAP_NUM_MAX      = 4
}osTrapType_t;

void default_ie_handler();
void default_ee_handler();
void default_int_handler();
void default_fault_handler();

/// Set trap handler of the current thread, generally need to be called when the thread is initialized
/// \param[in]     thread_id   thread ID obtained by \ref osThreadNew or \ref osThreadGetId; 
///                            if thread_id is NULL, current thread's handler will be set.
/// \param[in]     handler     trap handler function pointer.
/// \param[in]     type        trap type.
/// \return status code that indicates the execution status of the function.
osStatus_t osThreadSetTrapHandler(osThreadId_t thread_id, trap_handler h, osTrapType_t t);

/// Get trap handler of the current thread.
/// \param[in]     thread_id   thread ID obtained by \ref osThreadNew or \ref osThreadGetId;
///                            if thread_id is NULL, current thread's handler will be set.
/// \param[in]     type        trap type.
/// \return trap handler function pointer.
trap_handler osThreadGetTrapHandler(osThreadId_t thread_id, osTrapType_t t);

#endif