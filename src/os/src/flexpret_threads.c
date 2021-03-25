#include "cmsis_os2.h"
#include "flexpret_encoding.h"
#include "flexpret_threads.h"
#include "flexpret_utils.h"
#include "flexpret_timing.h"
#include "flexpret_io.h"
#include "flexpret_scheduler.h"

extern volatile hwthread_state startup_state[FLEXPRET_HW_THREADS_NUMS];
static int flexpret_thread_num = 1;
osThreadAttr_t *flexpret_thread_attr_entry[FLEXPRET_HW_THREADS_NUMS];
osThreadAttr_t flexpret_thread_attr[FLEXPRET_HW_THREADS_NUMS];
int default_soft_slot_id = -1;

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


__NO_RETURN void thread_after_return_handler() {
    uint32_t tid = read_csr(hartid);
    startup_state[tid].func = NULL;
    startup_state[tid].arg = NULL;
    startup_state[tid].stack_address = (void *)flexpret_thread_init_stack_addr[tid];
    // loop to sleep
    while (1) {
        uint32_t time = get_time();
        delay_until_periodic(&time, FLEXPRET_WAIT_PERIOD);
    }
}

// Thread cannot call thread_terminate to terminate itself
static void thread_terminate(osThreadId_t thread_id, int state_clear) {
    uint32_t tid = get_tid(thread_id);
    if (state_clear) {
        startup_state[tid].func = NULL;
        startup_state[tid].arg = NULL;
        startup_state[tid].stack_address = (void *)flexpret_thread_init_stack_addr[tid];
    }

    osPriority_t prior = flexpret_thread_attr_entry[tid]->priority;
    do {
        if (prior == osPriorityNormal) {
            osSchedulerSetSlotNum(thread_id, 0);
            uint32_t ss_num = osSchedulerGetSRTTNum();
            if (ss_num == 0) {
                osSchedulerSetSoftSlotNum(0);
            }
            break;
        } else if (prior == osPriorityRealtime) {
            osSchedulerSetSlotNum(thread_id, 0);
            break;
        } else {
            int tmode = osSchedulerGetTmodes(thread_id);
            if (tmode == TMODE_HA || tmode == TMODE_HZ) {
                prior = osPriorityRealtime;
            } else if (tmode == TMODE_SA || tmode == TMODE_SZ) {
                prior = osPriorityNormal;
            } else {
                flexpret_error("Bad priority\n");
                osSchedulerSetSlotNum(thread_id, 0);
                break;
            }
        }
    } while (prior == osPriorityNormal || prior == osPriorityRealtime);

    osSchedulerSetTmodes(thread_id, TMODE_ZOMBIE);
}

//  ==== Thread Management Functions ====
osThreadId_t osThreadNew (osThreadFunc_t func, void *argument, const osThreadAttr_t *attr) {
    if (flexpret_thread_num == FLEXPRET_HW_THREADS_NUMS) {
        flexpret_error("Max thread num exceed\n");
        return NULL;
    }
    // TODO: thread num inc should be an atomic operation.
    int tid = flexpret_thread_num++;

    if (attr == NULL) {
        // If attr is null, cb is stored at `flexpret_thread_attr`, stack is defined at startup.S
        osThreadAttr_t * attr_ptr = flexpret_thread_attr_entry[tid];
        memcpy(attr_ptr, &flexpret_thread_init_attr, sizeof(osThreadAttr_t));
        attr_ptr->stack_mem = (void*)flexpret_thread_init_stack_addr[tid];
        // tid will not be 0
        attr_ptr->stack_size = flexpret_thread_init_stack_addr[tid] - flexpret_thread_init_stack_addr[tid-1];
        // startup_state[tid].stack_address will be set in startup.S by default.
    } else {
        // If attr is not null, cb&stack is stored at user defined address
        flexpret_thread_attr_entry[tid] = attr;
        if (attr->stack_mem == NULL) {
            startup_state[tid].stack_address = (void *)flexpret_thread_init_stack_addr[tid];
            flexpret_thread_attr_entry[tid]->stack_mem = (void *)flexpret_thread_init_stack_addr[tid];
        } else {
            startup_state[tid].stack_address = attr->stack_mem;
            flexpret_thread_attr_entry[tid]->stack_mem = attr->stack_mem;
        }
        if (attr->stack_size == 0) {
            flexpret_thread_attr_entry[tid]->stack_size = flexpret_thread_init_stack_addr[tid] - flexpret_thread_init_stack_addr[tid-1];
        } else {
            flexpret_thread_attr_entry[tid]->stack_size = attr->stack_size;
        }
    }
    startup_state[tid].func = func;
    startup_state[tid].arg = argument;

    // Thread will not run until osKernelStart is called.
    return (osThreadId_t)&(startup_state[tid]);
}
 
const char *osThreadGetName (osThreadId_t thread_id) {
    // hwthread_state* tmp = (hwthread_state*) thread_id;
    uint32_t tid = get_tid(thread_id);
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
    uint32_t tid = get_tid(thread_id);
    switch (tmodes[tid])
    {
    // osThreadInactive
    // osThreadReady
    // osThreadRunning
    // osThreadBlocked
    // osThreadTerminate
    // TODO: map flexpret thread state to cmsis thread 
    case TMODE_HZ:
    case TMODE_SZ:
        return osThreadInactive;
    case TMODE_HA:
    case TMODE_SA:
        return osThreadRunning;
    default:
        return osThreadError;
    }
    return osThreadError;
}
 
uint32_t osThreadGetStackSize (osThreadId_t thread_id) {
    uint32_t tid = get_tid(thread_id);
    return flexpret_thread_attr_entry[tid]->stack_size;
}
 
uint32_t osThreadGetStackSpace (osThreadId_t thread_id) {
    uint32_t tid = get_tid(thread_id);
    uint32_t current_tid = read_csr(hartid);
    if (tid != current_tid) {
        flexpret_error("osThreadGetStackSpace only supports querying threads' own space\n");
        return 0;
    }
    uint32_t tid_addr = (uint32_t)&tid;
    uint32_t stack_mem = (uint32_t)flexpret_thread_attr_entry[tid]->stack_mem;
    uint32_t stack_size = flexpret_thread_attr_entry[tid]->stack_size;
    if (stack_mem > tid_addr) {
        if (stack_size > (stack_mem - tid_addr)) {
            return stack_size - (stack_mem - tid_addr);
        }
    }
    flexpret_error("Error calculating stack space");
    return 0;
}
 
osStatus_t osThreadSetPriority (osThreadId_t thread_id, osPriority_t priority) {
    uint32_t tid = get_tid(thread_id);
    osPriority_t current_priority = flexpret_thread_attr_entry[tid]->priority;
    if (current_priority == priority) {
        return osOK;
    }

    // TODO : set priority should be thread safe

    if (current_priority == osPriorityNormal) {
        // change SRTT to HRTT
        int32_t res = osSchedulerGetFreq(thread_id);
        int32_t freq = THREAD_FREQ(res);
        int32_t soft = SOFT_SLOT_COUNT(res);
        flexpret_thread_attr_entry[tid]->priority = osPriorityRealtime;
        if (freq != 0) {
            // if thread has its own slot, dont touch slot.
        } else {
            // thread doesnt have its own slot, alloc one.
            osSchedulerSetSlotNum(thread_id, 1);
        }
        osSchedulerSetTmodes(thread_id, TMODE_HARD);

        // if it was the only SRTT before, remove the soft slot
        int srtt_num = osSchedulerGetSRTTNum();
        if (srtt_num == 0 && soft != 0){
            osSchedulerSetSoftSlotNum(0);
        }
    } else if (current_priority == osPriorityRealtime) {
        // change HRTT to SRTT
        int32_t res = osSchedulerGetFreq(thread_id);
        int32_t freq = THREAD_FREQ(res);
        int32_t soft = SOFT_SLOT_COUNT(res);
        flexpret_thread_attr_entry[tid]->priority = osPriorityNormal;

        int srtt_num = osSchedulerGetSRTTNum();
        if (srtt_num == 0 && soft == 0) {
            osSchedulerSetSoftSlotNum(1);
        }
        osSchedulerSetTmodes(thread_id, TMODE_SOFT);
        if (freq == 0) {
            // if thread doesnt have its own slot, dont touch slot.
        } else {
            // thread have its own slot, remove it.
            // before removing, alloc a soft slot if there is no one (line 182) 
            osSchedulerSetSlotNum(thread_id, 0);
        }
    } else {
        flexpret_error("unsupported priority\n");
        return osError;
    }
    return osOK;
}
 
osPriority_t osThreadGetPriority (osThreadId_t thread_id) {
    uint32_t tid = get_tid(thread_id);
    return flexpret_thread_attr_entry[tid]->priority;
}
 
osStatus_t osThreadYield (void) {
    flexpret_not_supported(__func__);
    return osOK;
}
 
osStatus_t osThreadSuspend (osThreadId_t thread_id) {
    // TODO : Separate Inactive and Blocked state from HZ/SZ
    if (thread_id == osThreadGetId()) {
        // if du/wu/ie/ee instruction is called before, thread will be woked up when timer is due, use set_compare to avoid this behavior
        set_compare(0);
    } else {
        // Thread may be woked up
        flexpret_warn("[warn:osThreadSuspend] Suspended thread may be woked up by timer, if du/wu/ie/ee instruction is called before.\n");
    }
    osSchedulerSetTmodes(thread_id, TMODE_ZOMBIE);
    return osOK;
}
 
osStatus_t osThreadResume (osThreadId_t thread_id) {
    // TODO : Separate Inactive and Blocked state from HZ/SZ
    osSchedulerSetTmodes(thread_id, TMODE_ACTIVE);
    return osOK;
}
 
osStatus_t osThreadDetach (osThreadId_t thread_id) {
    uint32_t tid = get_tid(thread_id);
    flexpret_thread_attr_entry[tid]->attr_bits = osThreadDetached;
    return osOK;
}
 
osStatus_t osThreadJoin (osThreadId_t thread_id) {
    uint32_t tid = get_tid(thread_id);
    if (flexpret_thread_attr_entry[tid]->attr_bits != osThreadJoinable) {
        flexpret_error("Thread ");
        flexpret_error(itoa_hex_removing_ldz(tid));
        flexpret_error(" is not joinable\n");
        return osError;
    }
    uint32_t current_tid = read_csr(hartid);
    if (tid == current_tid) {
        flexpret_error("Thread cannot join itself.\n");
        return osError;
    }
    // TODO: using event/thread flag mechanism
    while (((hwthread_state*)thread_id)->func != NULL) {
        uint32_t time = get_time();
        delay_until_periodic(&time, FLEXPRET_WAIT_PERIOD);
    }

    // eventually terminate sub thread.
    thread_terminate(thread_id, 0);
    return osOK;
}
 
__NO_RETURN void osThreadExit (void) {
    thread_after_return_handler();
    // code will not reach here
    for (;;) {}
}
 
osStatus_t osThreadTerminate (osThreadId_t thread_id) {
    uint32_t tid = get_tid(thread_id);
    uint32_t current_tid = read_csr(hartid);
    if (tid == current_tid) {
        flexpret_error("Thread cannot terminate itself, use osThreadExit instead.\n");
        return osError;
    }
    thread_terminate(thread_id, 1);
    return osOK;
}
 
uint32_t osThreadGetCount (void) {
    int count = 0;
    register int i;
    for (i = 0; i < FLEXPRET_HW_THREADS_NUMS; i++) {
        if (startup_state[i].func != NULL) {
            count++;
        }
    }
    return count;
}
 
uint32_t osThreadEnumerate (osThreadId_t *thread_array, uint32_t array_items) {
    uint32_t count = 0;
    register uint32_t i;
    for (i = 0; i < FLEXPRET_HW_THREADS_NUMS && i < array_items; i++) {
        if (startup_state[i].func != NULL) {
            thread_array[count] = (osThreadId_t)(&startup_state[i]);
            count++;
        }
    }
    return count;
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
 