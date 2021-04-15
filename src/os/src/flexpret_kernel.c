#include "cmsis_os2.h"
#include "flexpret_kernel.h"
#include "flexpret_utils.h"
#include "flexpret_io.h"
#include "flexpret_encoding.h"
#include "flexpret_timing.h"
#include "flexpret_threads.h"
#include "flexpret_scheduler.h"
#include "flexpret_trap.h"
#include "flexpret_mutex.h"

extern osThreadAttr_t *flexpret_thread_attr_entry[FLEXPRET_HW_THREADS_NUMS];
extern osThreadAttr_t flexpret_thread_attr[FLEXPRET_HW_THREADS_NUMS];
extern osTimerAttr_t *flexpret_timer_attr_entry[FLEXPRET_HW_THREADS_NUMS];
extern volatile mutex_state startup_mutex_state[FLEXPRET_HW_THREADS_NUMS];
// extern osTimerAttr_t flexpret_timer_attr[FLEXPRET_HW_THREADS_NUMS];
extern const osThreadAttr_t flexpret_thread_init_attr;
extern volatile hwthread_state startup_state[FLEXPRET_HW_THREADS_NUMS];
extern const uint32_t flexpret_thread_init_stack_addr[FLEXPRET_HW_THREADS_NUMS];
// Only used to record the soft_slot_id at initialization time
extern int default_soft_slot_id;

// Extern global variable for kernel reset
extern int flexpret_thread_num;
extern int flexpret_mutex_num;
extern volatile trap_vector trap_handler_vector[FLEXPRET_HW_THREADS_NUMS];

// bss_end addr flag, defined in linker script
extern char bss_end;

osMutexId_t slot_mutex, tmode_mutex;
osKernelState_t kernel_state = osKernelInactive;

static const osMutexAttr_t slotMutexAttr = {
    "slotMutex", osMutexRecursive | osMutexRobust, NULL, 0U
};

static const osMutexAttr_t tmodeMutexAttr = {
    "tmodeMutex", osMutexRecursive | osMutexRobust, NULL, 0U
};

//  ==== Kernel Management Functions ====

osStatus_t osKernelInitialize (void) {
    // TODO: init

    // Init thread&timer attr entry
    register int i;
    for (i = 0; i < FLEXPRET_HW_THREADS_NUMS; i++) {
        flexpret_thread_attr_entry[i] = flexpret_thread_attr + i;
        flexpret_timer_attr_entry[i] = NULL;
    }

    // Init mutex
    for (i = 0; i < FLEXPRET_MUTEX_MAX_NUM; i++) {
        startup_mutex_state[i].owner = NULL;
        startup_mutex_state[i].ifRobust = 0;
        startup_mutex_state[i].ifRecursive = 0;
        startup_mutex_state[i].count = 0;
        startup_mutex_state[i].active = 0;
    }
    write_csr_marco(CSR_MUTEX_0, FLEXPRET_MUTEX_INACTIVE);
    write_csr_marco(CSR_MUTEX_1, FLEXPRET_MUTEX_INACTIVE);
    write_csr_marco(CSR_MUTEX_2, FLEXPRET_MUTEX_INACTIVE);
    write_csr_marco(CSR_MUTEX_3, FLEXPRET_MUTEX_INACTIVE);
    write_csr_marco(CSR_MUTEX_4, FLEXPRET_MUTEX_INACTIVE);
    write_csr_marco(CSR_MUTEX_5, FLEXPRET_MUTEX_INACTIVE);
    write_csr_marco(CSR_MUTEX_6, FLEXPRET_MUTEX_INACTIVE);
    write_csr_marco(CSR_MUTEX_7, FLEXPRET_MUTEX_INACTIVE);

    slot_mutex = osMutexNew(&slotMutexAttr);
    // tmode_mutex = osMutexNew(&tmodeMutexAttr);

    // Init thread 0
    memcpy(flexpret_thread_attr_entry[0], &flexpret_thread_init_attr, sizeof(osThreadAttr_t));
    flexpret_thread_attr_entry[0]->stack_mem = (void*)flexpret_thread_init_stack_addr[0];
    flexpret_thread_attr_entry[0]->stack_size = flexpret_thread_init_stack_addr[0] - (uint32_t)&bss_end;

    kernel_state = osKernelReady;
    startup_state[0].state = FLEXPRET_ACTIVE;
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
    return kernel_state;
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
            startup_state[i].state = FLEXPRET_ACTIVE;
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

    // set slots
    set_slots(slots);
    // set modes, reverse order
    set_tmodes(modes);
    kernel_state = osKernelRunning;
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
    // NOTE: in nanoseconds
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

osStatus_t osKernelReset() {
    // SLOT_D * 7, SLOT_0
    write_csr(badvaddr, 0xfffffff0);
    // TMODE_HZ * 7, TMODE_HA
    write_csr(ptbr, 0x55555554);
    register int i;
    for (i = 1; i < FLEXPRET_HW_THREADS_NUMS; i++) {
        startup_state[i].func = NULL;
        startup_state[i].stack_address = NULL;
        startup_state[i].arg = NULL;
        startup_state[i].state = FLEXPRET_INACTIVE;
    }

    // global variable reset
    flexpret_thread_num = 1;
    default_soft_slot_id = -1;
    flexpret_mutex_num = 0;
    for (i = 0; i < FLEXPRET_HW_THREADS_NUMS; i++) {
        trap_handler_vector[i].handler[0] = default_ie_handler;
        trap_handler_vector[i].handler[1] = default_ee_handler;
        trap_handler_vector[i].handler[2] = default_int_handler;
        trap_handler_vector[i].handler[3] = default_fault_handler;
    }

    return osKernelInitialize();
}