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
    const uint32_t ver = FLEXPRET_OS_VERSION;
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
    char buf[3][9];
	if ((id_buf != NULL) && (version != NULL)) {
        uint32_t v[3] = {
            KERNEL_VER_MAJOR(ver),
            KERNEL_VER_MINOR(ver),
            KERNEL_VER_PATCHLEVEL(ver)
        };
        int i = 0;
        for (; i < 3; i++) {
            char *tmp = itoa_hex_removing_ldz(v[i]);
            size_t s = strlen(tmp);
            strncpy(buf[i], tmp, s);
            if (id_size >= s) {
                strncpy(id_buf, buf[i], s);
                id_size -= s;
                id_buf += s;
            } else {
                strncpy(id_buf, buf[i], id_size);
                return osOK;
            }
            if (i != 2) {
                strncpy(id_buf, ".", 1);
                id_size -= 1;
                id_buf += 1;
            } else {
                strncpy(id_buf, "\n", 1);
                id_size -= 1;
                id_buf += 1;
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
