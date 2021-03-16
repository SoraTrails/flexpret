#include "cmsis_os2.h"
#include "flexpret_encoding.h"
#include "flexpret_threads.h"
#include "flexpret_utils.h"
#include "flexpret_timing.h"
#include "flexpret_io.h"

extern volatile hwthread_state startup_state[FLEXPRET_HW_THREADS_NUMS];
static int flexpret_thread_num = 1;
osThreadAttr_t *flexpret_thread_attr_entry[FLEXPRET_HW_THREADS_NUMS];
osThreadAttr_t flexpret_thread_attr[FLEXPRET_HW_THREADS_NUMS];

/********* OLD IMPLEMENTATION ***********
 * void hwthread_start(uint32_t tid, void (*func)(), uint32_t stack_address) {
 *     startup_state[tid].func = func;
 *     if(stack_address != NULL) {
 *         startup_state[tid].stack_address = stack_address;
 *     }
 * }
 * uint32_t hwthread_done(uint32_t tid) {
 *     return (startup_state[tid].func == NULL);
 * }
 */

const osThreadAttr_t flexpret_thread_init_attr = {
    .name = "FlexpretThread",
	.attr_bits = osThreadDetached,
	.cb_mem = NULL,
	.cb_size = 0,
	.stack_mem = NULL,
	.stack_size = 0,
	.priority = osPriorityNormal,
	.tz_module = 0,
	.reserved = 0,
};
const uint32_t flexpret_thread_init_stack_addr[FLEXPRET_HW_THREADS_NUMS] = {
    0x20001BFC,
    0x200027FC,
    0x200033FC,
    0x20003FFC
};

//  ==== Thread Management Functions ====
osThreadId_t osThreadNew (osThreadFunc_t func, void *argument, const osThreadAttr_t *attr) {
    // int thread_type;
    int attr_bits;
    if (flexpret_thread_num == FLEXPRET_HW_THREADS_NUMS) {
        flexpret_error("Max thread num exceed\n");
        return NULL;
    }
    if (attr == NULL) {
        // thread_type = soft;
        attr_bits = osThreadDetached;
    } else {
        // thread_type = attr->priority == osPriorityRealtime ? hard : soft;
        attr_bits = attr->attr_bits;
    }
    // TODO: thread num inc should be an atomic operation.
    int tid = flexpret_thread_num++;

    if (attr == NULL) {
        // If attr is null, cb is stored at `flexpret_thread_attr`, stack is defined at startup.S
        osThreadAttr_t * attr_ptr = flexpret_thread_attr_entry[tid];
        memcpy(attr_ptr, &flexpret_thread_init_attr, sizeof(osThreadAttr_t));

        // startup_state[tid].tid, startup_state[tid].stack_address will be set in startup.S by default.

    } else {
        // If attr is not null, cb&stack is stored at user defined address
        flexpret_thread_attr_entry[tid] = attr;
        if (attr->stack_mem == NULL) {
            startup_state[tid].stack_address = (void *)flexpret_thread_init_stack_addr[tid];
        } else {
            startup_state[tid].stack_address = attr->stack_mem;
        }
    }
    startup_state[tid].func = func;
    startup_state[tid].arg = argument;

    // Thread will not run until osKernelStart is called.
    return (osThreadId_t)&(startup_state[tid]);
}
 
const char *osThreadGetName (osThreadId_t thread_id) {
    // hwthread_state* tmp = (hwthread_state*) thread_id;
    uint32_t tmp = (uint32_t) thread_id;
    uint32_t tid = get_tid_by_offset(tmp - (uint32_t)startup_state);
    return flexpret_thread_attr_entry[tid]->name;
}
 
osThreadId_t osThreadGetId (void) {
    uint32_t tid = read_csr(hartid);
    return (osThreadId_t) (startup_state + tid);
}

osThreadState_t osThreadGetState (osThreadId_t thread_id) {
    uint32_t tmodes[FLEXPRET_HW_THREADS_NUMS];
    get_tmodes_4(tmodes, tmodes + 1, tmodes + 2, tmodes + 3);
    // hwthread_state* tmp = (hwthread_state*) thread_id;
    uint32_t tmp = (uint32_t) thread_id;
    uint32_t tid = get_tid_by_offset(tmp - (uint32_t)startup_state);
    return tmodes[tid];
}
 
uint32_t osThreadGetStackSize (osThreadId_t thread_id) {
    // hwthread_state* tmp = (hwthread_state*) thread_id;
    uint32_t tmp = (uint32_t) thread_id;
    uint32_t tid = get_tid_by_offset(tmp - (uint32_t)startup_state);
    return flexpret_thread_attr_entry[tid]->stack_size;
}
 
uint32_t osThreadGetStackSpace (osThreadId_t thread_id) {
    // TODO
    return 0;
}
 
osStatus_t osThreadSetPriority (osThreadId_t thread_id, osPriority_t priority) {
    // hwthread_state* tmp = (hwthread_state*) thread_id;
    uint32_t tmp = (uint32_t) thread_id;
    uint32_t tid = get_tid_by_offset(tmp - (uint32_t)startup_state);
    osPriority_t current_priority = flexpret_thread_attr_entry[tid]->priority;
    if (current_priority == priority) {
        return osOK;
    }
    if (current_priority == osPriorityNormal) {
        flexpret_thread_attr_entry[tid]->priority = osPriorityNormal;
        // TODO: modify tmode csr if is hw thread, otherwise ...
    } else if (current_priority == osPriorityRealtime) {
        flexpret_thread_attr_entry[tid]->priority = osPriorityRealtime;
        // TODO
    } else {
        flexpret_error("unsupported priority");
        return osError;
    }
    return osOK;
}
 
osPriority_t osThreadGetPriority (osThreadId_t thread_id) {
    // hwthread_state* tmp = (hwthread_state*) thread_id;
    uint32_t tmp = (uint32_t) thread_id;
    uint32_t tid = get_tid_by_offset(tmp - (uint32_t)startup_state);
    return flexpret_thread_attr_entry[tid]->priority;
}
 
osStatus_t osThreadYield (void) {
    // TODO
    return osOK;
}
 
osStatus_t osThreadSuspend (osThreadId_t thread_id) {
    // TODO
    return osOK;
}
 
osStatus_t osThreadResume (osThreadId_t thread_id) {
    // TODO
    return osOK;
}
 
osStatus_t osThreadDetach (osThreadId_t thread_id) {
    // hwthread_state* tmp = (hwthread_state*) thread_id;
    uint32_t tmp = (uint32_t) thread_id;
    uint32_t tid = get_tid_by_offset(tmp - (uint32_t)startup_state);
    flexpret_thread_attr_entry[tid]->attr_bits = osThreadDetached;
    return osOK;
}
 
osStatus_t osThreadJoin (osThreadId_t thread_id) {
    // hwthread_state* tmp = (hwthread_state*) thread_id;
    uint32_t tmp = (uint32_t) thread_id;
    uint32_t tid = get_tid_by_offset(tmp - (uint32_t)startup_state);
    if (flexpret_thread_attr_entry[tid]->attr_bits != osThreadJoinable) {
        flexpret_error("Thread ");
        flexpret_error(itoa_hex_removing_ldz(tid));
        flexpret_error(" is not joinable\n");
        return osError;
    }
    // TODO: using event/thread flag mechanism
    while (((hwthread_state*)tmp)->func != NULL) {
        uint32_t time = get_time();
        delay_until_periodic(&time, FLEXPRET_WAIT_PERIOD);
    }

    // eventually terminate sub thread.
    thread_terminate(tid);
    return osOK;
}
 
__NO_RETURN void osThreadExit (void) {
    // TODO
    for (;;);
}
 
osStatus_t osThreadTerminate (osThreadId_t thread_id) {
    // TODO
    return osOK;
}
 
uint32_t osThreadGetCount (void) {
    // TODO
    return 0;
}
 
uint32_t osThreadEnumerate (osThreadId_t *thread_array, uint32_t array_items) {
    // TODO
    return 0;
}

//  ==== Generic Wait Functions ====
 
osStatus_t osDelay (uint32_t ticks) {
    uint32_t time = get_time();
    delay_until_periodic(&time, ticks);
    return osOK;
}
 
osStatus_t osDelayUntil (uint32_t ticks) {
    set_compare(ticks);
    delay_until();
    return osOK;
}
 