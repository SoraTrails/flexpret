#include "flexpret_scheduler.h"
#include "flexpret_encoding.h"
#include "flexpret_threads.h"

extern volatile hwthread_state startup_state[FLEXPRET_HW_THREADS_NUMS];
extern osThreadAttr_t *flexpret_thread_attr_entry[FLEXPRET_HW_THREADS_NUMS];
extern osMutexId_t slot_mutex;
extern osMutexId_t tmode_mutex;

int32_t osSchedulerGetFreq(osThreadId_t thread_id) {
    uint8_t slots[FLEXPRET_MAX_HW_THREADS_NUMS];
    get_slots(slots);
    register int i;
    uint8_t soft_count = 0;
    uint8_t active_count = 0;
    uint8_t thread_count = 0;
    uint32_t tid = get_tid(thread_id);

    for(i = 0; i < FLEXPRET_MAX_HW_THREADS_NUMS; i++) {
        if (slots[i] == tid) {
            thread_count++;
        } else if (slots[i] == SLOT_S) {
            soft_count++;
        }
        if (slots[i] != SLOT_D) {
            active_count++;
        }
    }
    return ((thread_count & 0xF) << 8) | ((soft_count & 0xF) << 4) | (active_count & 0xF);
}


static osStatus_t set_slot_num (int tid, int count) {
    uint8_t slots[FLEXPRET_MAX_HW_THREADS_NUMS];

    get_slots(slots);
    register int i;
    int32_t active_count = 0;
    int32_t thread_count = 0;
    for(i = 0; i < FLEXPRET_MAX_HW_THREADS_NUMS; i++) {
        if (slots[i] == tid) {
            active_count++;
            thread_count++;
        } else if (slots[i] != SLOT_D) {
            active_count++;
        }
    }

    if (count == thread_count) {
        return osOK;
    } else if (count > thread_count) {
        // add `count - thread_count` slot
        int32_t slot_to_be_added = count - thread_count;
        if (active_count + slot_to_be_added > FLEXPRET_MAX_HW_THREADS_NUMS - 1) {
            flexpret_error("Bad slot count");
            return osError;
        }
        for(i = 0; i < FLEXPRET_MAX_HW_THREADS_NUMS; i++) {
            if (slots[i] == SLOT_D) {
                slots[i] = tid;
                slot_to_be_added--;
                if (slot_to_be_added == 0) {
                    break;
                }
            }
        }
    } else {
        // reduce `thread_count - count` slot
        int32_t slot_to_be_reduced = thread_count - count;
        for(i = 0; i < FLEXPRET_MAX_HW_THREADS_NUMS; i++) {
            if (slots[i] == tid) {
                slots[i] = SLOT_D;
                slot_to_be_reduced--;
                if (slot_to_be_reduced == 0) {
                    break;
                }
            }
        }
    }
    set_slots(slots);
    return osOK;
}

// TODO: It’s better to make the same thread's slots be interleaved
osStatus_t osSchedulerSetSlotNum(osThreadId_t thread_id, int count) {
    if (count < 0 || count >= FLEXPRET_MAX_HW_THREADS_NUMS - 1) {
        return osErrorParameter;
    }
    uint32_t tid = get_tid(thread_id);
    if (osMutexAcquire(slot_mutex, osWaitForever) != osOK) {
        flexpret_error("Error acquire slot mutex.\n");
    }
    uint32_t ret = set_slot_num(tid, count);
    if (osMutexRelease(slot_mutex) != osOK) {
        flexpret_error("Error release slot mutex.\n");
    }
    return ret;
}

osStatus_t osSchedulerSetSoftSlotNum(int count) {
    if (count < 0 || count >= FLEXPRET_MAX_HW_THREADS_NUMS - 1) {
        return osErrorParameter;
    }

    if (osMutexAcquire(slot_mutex, osWaitForever) != osOK) {
        flexpret_error("Error acquire slot mutex.\n");
    }
    uint32_t ret = set_slot_num(SLOT_S, count);
    if (osMutexRelease(slot_mutex) != osOK) {
        flexpret_error("Error release slot mutex.\n");
    }
    return ret;
}

static osStatus_t set_thread_mode(uint32_t tid, int tmode) {
    uint32_t hbit = 1 << ((tid << 1) + 1);
    uint32_t lbit = 1 << ((tid << 1) + 0);
    switch (tmode) {
    case TMODE_HARD:
        clear_csr(ptbr, hbit);
        break;
    case TMODE_SOFT:
        set_csr(ptbr, hbit);
        break;
    case TMODE_ACTIVE:
        clear_csr(ptbr, lbit);
        break;
    case TMODE_ZOMBIE:
        set_csr(ptbr, lbit);
        break;
    case TMODE_SZ:
        set_csr(ptbr, hbit);
        set_csr(ptbr, lbit);
        break;
    case TMODE_SA:
        set_csr(ptbr, hbit);
        clear_csr(ptbr, lbit);
        break;
    case TMODE_HZ:
        clear_csr(ptbr, hbit);
        set_csr(ptbr, lbit);
        break;
    case TMODE_HA:
        clear_csr(ptbr, hbit);
        clear_csr(ptbr, lbit);
        break;
    default:
        break;
    }
    return osOK;
}

osStatus_t osSchedulerSetTmodes(osThreadId_t thread_id, int tmode) {
    uint32_t tid = get_tid(thread_id);
    // if (osMutexAcquire(tmode_mutex, osWaitForever) != osOK) {
    //     flexpret_error("Error acquire tmode mutex\n");
    // }
    osStatus_t ret = set_thread_mode(tid, tmode);
    // if (osMutexRelease(tmode_mutex) != osOK) {
    //     flexpret_error("Error release tmode mutex\n");
    // }
    return ret;
}

int osSchedulerGetTmodes(osThreadId_t thread_id) {
    uint32_t tid = get_tid(thread_id);

    uint32_t tmodes[FLEXPRET_HW_THREADS_NUMS];
    get_tmodes(tmodes);
    return tmodes[tid];
}

int32_t osSchedulerGetSRTTNum() {
    uint32_t tmodes[FLEXPRET_HW_THREADS_NUMS];
    get_tmodes(tmodes);
    register int i;
    int res = 0;
    for (i = 0; i < FLEXPRET_HW_THREADS_NUMS; i++) {
        if (tmodes[i] == TMODE_SA || tmodes[i] == TMODE_SZ) {
            if (startup_state[i].func != NULL) {
                res++;
            }
        }
    }
    return res;
}

static void acquire_slot_mutex() {
    osMutexAcquire(slot_mutex, osWaitForever);
}

static void release_slot_mutex() {
    osMutexRelease(slot_mutex);
}

void srtt_terminate(uint32_t tid) {
    acquire_slot_mutex();
    
    uint32_t tmodes[FLEXPRET_HW_THREADS_NUMS];
    get_tmodes(tmodes);
    register int i;
    int res = 0;
    for (i = 0; i < FLEXPRET_HW_THREADS_NUMS; i++) {
        if (i == tid) {
            continue;
        }
        if (tmodes[i] == TMODE_SA || tmodes[i] == TMODE_SZ) {
            if (startup_state[i].func != NULL) {
                res++;
            }
        }
    }
    set_slot_num(tid, 0);
    if (res == 0) {
        set_slot_num(SLOT_S, 0);
    }

    release_slot_mutex();
}

void change_srtt_to_hrtt(uint32_t tid) {
    acquire_slot_mutex();
    
    uint8_t slots[FLEXPRET_MAX_HW_THREADS_NUMS];
    get_slots(slots);

    uint32_t tmodes[FLEXPRET_HW_THREADS_NUMS];
    get_tmodes(tmodes);

    register int i;
    int32_t soft_count = 0;
    int32_t thread_count = 0;
    for(i = 0; i < FLEXPRET_MAX_HW_THREADS_NUMS; i++) {
        if (slots[i] == tid) {
            thread_count++;
        } else if (slots[i] == SLOT_S) {
            soft_count++;
        }
    }
    int srtt_num = 0;
    for (i = 0; i < FLEXPRET_HW_THREADS_NUMS; i++) {
        if (i == tid) {
            continue;
        }
        if (tmodes[i] == TMODE_SA || tmodes[i] == TMODE_SZ) {
            if (startup_state[i].func != NULL) {
                srtt_num++;
            }
        }
    }
    if (thread_count == 0) {
        set_slot_num(tid, 1);
    }
    set_thread_mode(tid, TMODE_HARD);

    if (srtt_num == 0 && soft_count != 0) {
        set_slot_num(SLOT_S, 0);
    }
    
    release_slot_mutex();
}

void change_hrtt_to_srtt(uint32_t tid) {
    acquire_slot_mutex();

    uint8_t slots[FLEXPRET_MAX_HW_THREADS_NUMS];
    get_slots(slots);

    uint32_t tmodes[FLEXPRET_HW_THREADS_NUMS];
    get_tmodes(tmodes);

    register int i;
    int32_t soft_count = 0;
    int32_t thread_count = 0;
    for(i = 0; i < FLEXPRET_MAX_HW_THREADS_NUMS; i++) {
        if (slots[i] == tid) {
            thread_count++;
        } else if (slots[i] == SLOT_S) {
            soft_count++;
        }
    }
    int srtt_num = 0;
    for (i = 0; i < FLEXPRET_HW_THREADS_NUMS; i++) {
        if (i == tid) {
            continue;
        }
        if (tmodes[i] == TMODE_SA || tmodes[i] == TMODE_SZ) {
            if (startup_state[i].func != NULL) {
                srtt_num++;
            }
        }
    }
    if (srtt_num == 0 && soft_count == 0) {
        set_slot_num(SLOT_S, 1);
    }
    set_thread_mode(tid, TMODE_SOFT);
    if (thread_count != 0) {
        set_slot_num(tid, 0);
    }

    release_slot_mutex();
}

void thread_after_create(uint32_t tid) {
    acquire_slot_mutex();

    if (flexpret_thread_attr_entry[tid]->priority == osPriorityRealtime) {
        set_thread_mode(tid, TMODE_HA);
        set_slot_num(tid, 1);
    } else if (flexpret_thread_attr_entry[tid]->priority == osPriorityNormal) {
        set_thread_mode(tid, TMODE_SA);
        uint8_t slots[FLEXPRET_MAX_HW_THREADS_NUMS];
        get_slots(slots);

        int32_t soft_count = 0;
        if (startup_state[tid].func != NULL) {
            register int i;
            for (i = 0; i < FLEXPRET_MAX_HW_THREADS_NUMS; i++) {
                if (slots[i] == SLOT_S) {
                    soft_count++;
                }
            }
        } else {
            flexpret_error("Internal error\n");
        }
        if (soft_count == 0) {
            set_slot_num(SLOT_S, 1);
        }
    } else {
        flexpret_error("Unsupported priority\n");
    }

    release_slot_mutex();
}
