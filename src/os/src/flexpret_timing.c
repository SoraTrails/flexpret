#include "cmsis_os2.h"
#include "flexpret_timing.h"
#include "flexpret_threads.h"
#include "flexpret_utils.h"
#include "flexpret_trap.h"
#include "flexpret_io.h"

extern volatile hwtimer_state startup_timer_state[FLEXPRET_HW_THREADS_NUMS];
static osTimerAttr_t flexpret_timer_attr[FLEXPRET_HW_THREADS_NUMS];
osTimerAttr_t *flexpret_timer_attr_entry[FLEXPRET_HW_THREADS_NUMS];

const osTimerAttr_t flexpret_timer_init_attr = {
    .name = "Thread timer",
	.attr_bits = 0,
	.cb_mem = NULL,
	.cb_size = sizeof(hwtimer_state)
};

static void timer_handler() {
    uint32_t tid = read_csr(hartid);
    volatile hwtimer_state* tmp = startup_timer_state + tid;

    tmp->func(tmp->arg);

    tmp->ticks = 0;
}

static void timer_periodic_handler() {
    uint32_t tid = read_csr(hartid);
    volatile hwtimer_state* tmp = startup_timer_state + tid;

    tmp->func(tmp->arg);
    
    uint32_t time = get_time();
    set_compare(tmp->ticks + time);
    interrupt_expire();
}

//  ==== Timer Management Functions ====

osTimerId_t osTimerNew (osTimerFunc_t func, osTimerType_t type, void *argument, const osTimerAttr_t *attr) {
    uint32_t tid = read_csr(hartid);
    // A timer is active <=> `flexpret_timer_attr_entry[tid] != NULL`
    if (flexpret_timer_attr_entry[tid] != NULL) {
        flexpret_error("A thread can ONLY create one active timer because of Flexpret's timing mechanism.\n");
        return NULL;
    }
    if (attr == NULL) {
        flexpret_timer_attr_entry[tid] = flexpret_timer_attr + tid;
        osTimerAttr_t * attr_ptr = flexpret_timer_attr_entry[tid];
        memcpy(attr_ptr, &flexpret_timer_init_attr, sizeof(osTimerAttr_t));
        attr_ptr->cb_mem = (void*)(startup_timer_state + tid);
    } else {
        flexpret_timer_attr_entry[tid] = attr;
    }

    startup_timer_state[tid].func = func;
    startup_timer_state[tid].arg = argument;
    startup_timer_state[tid].type = type;
    startup_timer_state[tid].ticks = 0;
    
    return (osTimerId_t)&(startup_timer_state[tid]);
}
 
const char *osTimerGetName (osTimerId_t timer_id) {
    uint32_t tid = get_timer_tid(timer_id);
    if (flexpret_timer_attr_entry[tid] == NULL) {
        flexpret_error("Timer is not active\n");
        return "";
    }

    return flexpret_timer_attr_entry[tid]->name;
}
 
osStatus_t osTimerStart (osTimerId_t timer_id, uint32_t ticks) {
    uint32_t tid = get_timer_tid(timer_id);
    if (flexpret_timer_attr_entry[tid] == NULL) {
        flexpret_error("Timer is not active.\n");
        return osError;
    }
    if (ticks == 0) {
        flexpret_error("Ticks should be greater than 0.\n");
        return osError;
    }
    volatile hwtimer_state * timer = (hwtimer_state *)timer_id;

    if (timer->type == osTimerOnce) {
        osThreadSetTrapHandler(osThreadGetId(), (trap_handler)timer_handler, InterruptOnExpire);
    } else if (timer->type == osTimerPeriodic) {
        osThreadSetTrapHandler(osThreadGetId(), (trap_handler)timer_periodic_handler, InterruptOnExpire);
    } else {
        flexpret_error("Bad timer type.\n");
        return osError;
    }

    // A timer is running <=> `timer->ticks != 0`
    timer->ticks = ticks;

    uint32_t time = get_time();
    set_compare(ticks + time);
    interrupt_expire();
    return osOK;
}

osStatus_t osTimerStop (osTimerId_t timer_id) {
    uint32_t tid = get_timer_tid(timer_id);
    if (flexpret_timer_attr_entry[tid] == NULL) {
        flexpret_error("Timer is not active.\n");
        return osError;
    }
    volatile hwtimer_state * timer = (hwtimer_state *)timer_id;
    timer->ticks = 0;
    set_compare(0);
    osThreadSetTrapHandler(osThreadGetId(), (trap_handler)default_ie_handler, InterruptOnExpire);
    return osOK;
}
 
uint32_t osTimerIsRunning (osTimerId_t timer_id) {
    if (timer_id == NULL) {
        return 0;
    }
    uint32_t tid = get_timer_tid(timer_id);
    if (flexpret_timer_attr_entry[tid] == NULL) {
        flexpret_error("Timer is not active.\n");
        return 0;
    }

    volatile hwtimer_state * timer = (hwtimer_state *)timer_id;
    // A timer is running <=> `timer->ticks != 0`
    return timer->ticks != 0;
}
 
osStatus_t osTimerDelete (osTimerId_t timer_id) {
    uint32_t tid = get_timer_tid(timer_id);
    volatile hwtimer_state * timer = (hwtimer_state *)timer_id;
    // stop running timer before deleting it
    if (timer->ticks != 0) {
        timer->ticks = 0;
        set_compare(0);
    }

    flexpret_timer_attr_entry[tid] = NULL;

    startup_timer_state[tid].func = NULL;
    startup_timer_state[tid].arg = NULL;
    startup_timer_state[tid].ticks = 0;

    return osOK;
}
 
osTimerId_t osThreadGetTimer (osThreadId_t thread_id) {
    uint32_t tid = get_tid(thread_id);
    if (flexpret_timer_attr_entry[tid] == NULL) {
        return NULL;
    }
    return (osTimerId_t)&(startup_timer_state[tid]);
}

osStatus_t osTimerSetFunc (osTimerId_t timer, osTimerFunc_t timer_func, void* argument) {
    uint32_t tid = get_timer_tid(timer);
    // A timer is active <=> `flexpret_timer_attr_entry[tid] != NULL`
    if (flexpret_timer_attr_entry[tid] == NULL) {
        flexpret_error("Timer is not active.\n");
        return osError;
    }
    uint32_t ctid = read_csr(hartid);
    if (tid != ctid) {
        flexpret_error("Timer does not belong to current thread.\n");
        return osError;
    }
    set_compare(0);
    startup_timer_state[tid].func = timer_func;
    startup_timer_state[tid].arg = argument;
    startup_timer_state[tid].ticks = 0;
    return osOK;
}
