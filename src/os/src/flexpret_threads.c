#include "cmsis_os2.h"
#include "flexpret_threads.h"

extern volatile hwthread_state startup_state[THREADS];

void hwthread_start(uint32_t tid, void (*func)(), uint32_t stack_address) {
    startup_state[tid].func = func;
    if(stack_address != NULL) {
        startup_state[tid].stack_address = stack_address;
    }
}

uint32_t hwthread_done(uint32_t tid) {
    return (startup_state[tid].func == NULL);
}

//  ==== Thread Management Functions ====

osThreadId_t osThreadNew (osThreadFunc_t func, void *argument, const osThreadAttr_t *attr) {
    return NULL;
}
 
const char *osThreadGetName (osThreadId_t thread_id) {
    return NULL;
}
 
osThreadId_t osThreadGetId (void) {
    return NULL;
}
 
osThreadState_t osThreadGetState (osThreadId_t thread_id) {
    return osThreadReserved;
}
 
uint32_t osThreadGetStackSize (osThreadId_t thread_id) {
    return 0;
}
 
uint32_t osThreadGetStackSpace (osThreadId_t thread_id) {
    return 0;
}
 
osStatus_t osThreadSetPriority (osThreadId_t thread_id, osPriority_t priority) {
    return osOK;
}
 
osPriority_t osThreadGetPriority (osThreadId_t thread_id) {
    return osPriorityReserved;
}
 
osStatus_t osThreadYield (void) {
    return osOK;
}
 
osStatus_t osThreadSuspend (osThreadId_t thread_id) {
    return osOK;
}
 
osStatus_t osThreadResume (osThreadId_t thread_id) {
    return osOK;
}
 
osStatus_t osThreadDetach (osThreadId_t thread_id) {
    return osOK;
}
 
osStatus_t osThreadJoin (osThreadId_t thread_id) {
    return osOK;
}
 
__NO_RETURN void osThreadExit (void) {
    for (;;);
}
 
osStatus_t osThreadTerminate (osThreadId_t thread_id) {
    return osOK;
}
 
uint32_t osThreadGetCount (void) {
    return 0;
}
 
uint32_t osThreadEnumerate (osThreadId_t *thread_array, uint32_t array_items) {
    return 0;
}
 
 
//  ==== Thread Flags Functions ====
 
uint32_t osThreadFlagsSet (osThreadId_t thread_id, uint32_t flags) {
    return 0;
}
 
uint32_t osThreadFlagsClear (uint32_t flags) {
    return 0;
}
 
uint32_t osThreadFlagsGet (void) {
    return 0;
}
 
uint32_t osThreadFlagsWait (uint32_t flags, uint32_t options, uint32_t timeout) {
    return 0;
}
 
 
//  ==== Generic Wait Functions ====
 
osStatus_t osDelay (uint32_t ticks) {
    return osOK;
}
 
osStatus_t osDelayUntil (uint32_t ticks) {
    return osOK;
}
 