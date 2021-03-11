#include "cmsis_os2.h"

//  ==== Kernel Management Functions ====

osStatus_t osKernelInitialize (void) {
    return osOK;
}

osStatus_t osKernelGetInfo (osVersion_t *version, char *id_buf, uint32_t id_size) {
    return osOK;
}

osKernelState_t osKernelGetState (void) {
    return osKernelReserved;
}

osStatus_t osKernelStart (void) {
    return osOK;
}

int32_t osKernelLock (void) {
    return osOK;
}

int32_t osKernelUnlock (void) {
    return osOK;
}

int32_t osKernelRestoreLock (int32_t lock) {
    return osOK;
}

uint32_t osKernelSuspend (void) {
    return osOK;
}

void osKernelResume (uint32_t sleep_ticks) {

}

uint32_t osKernelGetTickCount (void) {
    return osOK;
}
 
uint32_t osKernelGetTickFreq (void) {
    return osOK;
}
 
uint32_t osKernelGetSysTimerCount (void) {
    return osOK;
}
 
uint32_t osKernelGetSysTimerFreq (void) {
    return osOK;
}
