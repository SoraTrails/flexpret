#include "cmsis_os2.h"
#include "flexpret_kernel.h"
#include "flexpret_utils.h"
#include "flexpret_io.h"


//  ==== Kernel Management Functions ====

osStatus_t osKernelInitialize (void) {
    // init
    return osOK;
}

osStatus_t osKernelGetInfo (osVersion_t *version, char *id_buf, uint32_t id_size) {
    uint32_t ver = FLEXPRET_OS_VERSION;
    if (version != NULL) {
        version->api = ver;
        version->kernel = ver;
    } else {
        return osError;
    }
    if (id_size == 1 && id_buf != NULL) {
        *id_buf = '\0';
    }
    
    // For ending '\0'
    id_size -= 1;

	if ((id_buf != NULL) && (version != NULL)) {
        const char *v[3] = { 
            itoa_hex(KERNEL_VER_MAJOR(ver)),
            itoa_hex(KERNEL_VER_MINOR(ver)),
            itoa_hex(KERNEL_VER_PATCHLEVEL(ver))
        };
        uint32_t s[3] = { 
            strlen(v[0]),
            strlen(v[1]),
            strlen(v[2])
        };
        int i = 0;
        for (; i < 3; i++) {
            if (id_size >= s[i]) {
                strncpy(id_buf, v[i], s[i]);
                id_size -= s[i];
                id_buf += s[i];
            } else {
                strncpy(id_buf, v[i], id_size);
                return osOK;
            }
        }
	} else {
        return osError;
    }
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
