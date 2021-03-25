#include "cmsis_os2.h"
#include "flexpret_kernel.h"
#include "flexpret_utils.h"
#include "flexpret_io.h"
#include "flexpret_encoding.h"
#include "flexpret_timing.h"
#include "flexpret_threads.h"
#include "flexpret_scheduler.h"
#include "flexpret_trap.h"

extern osThreadAttr_t *flexpret_thread_attr_entry[FLEXPRET_HW_THREADS_NUMS];
extern osThreadAttr_t flexpret_thread_attr[FLEXPRET_HW_THREADS_NUMS];
extern const osThreadAttr_t flexpret_thread_init_attr;
extern volatile hwthread_state startup_state[FLEXPRET_HW_THREADS_NUMS];
extern const uint32_t flexpret_thread_init_stack_addr[FLEXPRET_HW_THREADS_NUMS];
// Only used to record the soft_slot_id at initialization time
extern int default_soft_slot_id;
// bss_end addr flag, defined in linker script
extern char bss_end;

//  ==== Kernel Management Functions ====

osStatus_t osKernelInitialize (void) {
    // TODO: init

    // Init thread attr entry
    register int i;
    for (i = 0; i < FLEXPRET_HW_THREADS_NUMS; i++) {
        flexpret_thread_attr_entry[i] = flexpret_thread_attr + i;
    }

    // Init thread 0
    memcpy(flexpret_thread_attr_entry[0], &flexpret_thread_init_attr, sizeof(osThreadAttr_t));
    flexpret_thread_attr_entry[0]->stack_mem = (void*)flexpret_thread_init_stack_addr[0];
    flexpret_thread_attr_entry[0]->stack_size = flexpret_thread_init_stack_addr[0] - (uint32_t)&bss_end;

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
    flexpret_not_implemented(__func__);
    return osKernelReserved;
}

osStatus_t osKernelStart (void) {
    // run all threads
    register int i;
    int slots[FLEXPRET_MAX_HW_THREADS_NUMS];
    int modes[FLEXPRET_HW_THREADS_NUMS];
    slots[0] = SLOT_T0;
    modes[0] = TMODE_HA;
    for (i = 1; i < FLEXPRET_HW_THREADS_NUMS; i++) {
        if (startup_state[i].func != NULL) {
            // slots[i] = i; // SLOT_Ti
            // TODO soft不占slot？
            if (flexpret_thread_attr_entry[i]->priority == osPriorityRealtime) {
                modes[i] = TMODE_HA;
                slots[i] = i;
            } else if (flexpret_thread_attr_entry[i]->priority == osPriorityNormal) {
                modes[i] = TMODE_SA;
                if (default_soft_slot_id == -1) {
                    slots[i] = SLOT_S;
                    default_soft_slot_id = i;
                } else {
                    slots[i] = SLOT_D;
                }
            } else {
                flexpret_error("Unsupported priority\n");
                modes[i] = TMODE_SZ;
                slots[i] = SLOT_D;
            }
        } else {
            slots[i] = SLOT_D;
            modes[i] = TMODE_SZ;
        }
    }
    for (i = FLEXPRET_HW_THREADS_NUMS; i < FLEXPRET_MAX_HW_THREADS_NUMS; i++) {
        slots[i] = SLOT_D;
    }

#if FLEXPRET_HW_THREADS_NUMS != 4
#error FLEXPRET_HW_THREADS_NUMS should be 4 currently.
#endif

    // set slots
    set_slots(slots[7], slots[6], slots[5], slots[4], slots[3], slots[2], slots[1], slots[0]);
    // set modes, reverse order
    set_tmodes_4(modes[3], modes[2], modes[1], modes[0]);

    return osOK;
}

int32_t osKernelLock (void) {
    flexpret_not_implemented(__func__);
    return osOK;
}

int32_t osKernelUnlock (void) {
    flexpret_not_implemented(__func__);
    return osOK;
}

int32_t osKernelRestoreLock (int32_t lock) {
    flexpret_not_implemented(__func__);
    return osOK;
}

uint32_t osKernelSuspend (void) {
    flexpret_not_implemented(__func__);
    return osOK;
}

void osKernelResume (uint32_t sleep_ticks) {
    flexpret_not_implemented(__func__);
}

uint32_t osKernelGetTickCount (void) {
    return get_time();
}
 
uint32_t osKernelGetTickFreq (void) {
    return FLEXPRET_FREQ;
}
 
uint32_t osKernelGetSysTimerCount (void) {
    flexpret_not_implemented(__func__);
    return osOK;
}
 
uint32_t osKernelGetSysTimerFreq (void) {
    flexpret_not_implemented(__func__);
    return osOK;
}
