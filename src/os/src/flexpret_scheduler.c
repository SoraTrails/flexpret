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
    uint32_t tmodes[FLEXPRET_HW_THREADS_NUMS];
    get_tmodes(tmodes);
    switch (tmode) {
    case TMODE_HARD:
        if (tmodes[tid] == TMODE_HZ || tmodes[tid] == TMODE_HA) {
            return osOK;
        } else if (tmodes[tid] == TMODE_SZ) {
            tmodes[tid] = TMODE_HZ;
        } else if (tmodes[tid] == TMODE_SA) {
            tmodes[tid] = TMODE_HA;
        } else {
            flexpret_error("Bad tmode.\n");
            return osError;
        }
        break;
    case TMODE_SOFT:
        if (tmodes[tid] == TMODE_SZ || tmodes[tid] == TMODE_SA) {
            return osOK;
        } else if (tmodes[tid] == TMODE_HZ) {
            tmodes[tid] = TMODE_SZ;
        } else if (tmodes[tid] == TMODE_HA) {
            tmodes[tid] = TMODE_SA;
        } else {
            flexpret_error("Bad tmode.\n");
            return osError;
        }
        break;
    case TMODE_ACTIVE:
        if (tmodes[tid] == TMODE_SA || tmodes[tid] == TMODE_HA) {
            return osOK;
        } else if (tmodes[tid] == TMODE_HZ) {
            tmodes[tid] = TMODE_HA;
        } else if (tmodes[tid] == TMODE_SZ) {
            tmodes[tid] = TMODE_SA;
        } else {
            flexpret_error("Bad tmode.\n");
            return osError;
        }
        break;
    case TMODE_ZOMBIE:
        if (tmodes[tid] == TMODE_SZ || tmodes[tid] == TMODE_HZ) {
            return osOK;
        } else if (tmodes[tid] == TMODE_HA) {
            tmodes[tid] = TMODE_HZ;
        } else if (tmodes[tid] == TMODE_SA) {
            tmodes[tid] = TMODE_SZ;
        } else {
            flexpret_error("Bad tmode.\n");
            return osError;
        }
        break;
    case TMODE_SZ:
    case TMODE_SA:
    case TMODE_HZ:
    case TMODE_HA:
        tmodes[tid] = tmode;
        break;
    default:
        flexpret_error("Bad tmode.\n");
        return osError;
    }
    set_tmodes(tmodes);
    return osOK;
}

osStatus_t osSchedulerSetTmodes(osThreadId_t thread_id, int tmode) {
    uint32_t tid = get_tid(thread_id);

    if (osMutexAcquire(tmode_mutex, osWaitForever) != osOK) {
        flexpret_error("Error acquire tmode mutex\n");
    }
    
    osStatus_t ret = set_thread_mode(tid, tmode);

    if (osMutexRelease(tmode_mutex) != osOK) {
        flexpret_error("Error release tmode mutex\n");
    }
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

static void acquire_slot_tmode_mutex() {
    osStatus_t tmode_res, slot_res;
    while(1) {
        tmode_res = osMutexAcquire(tmode_mutex, 0);
        slot_res = osMutexAcquire(slot_mutex, 0);
        if (tmode_res == osOK && slot_res == osOK) {
            break;
        } else {
            if (slot_res == osOK) {
                osMutexRelease(slot_mutex);
            }
            if (tmode_res == osOK) {
                osMutexRelease(tmode_mutex);
            }       
        }
    }
}

static void release_slot_tmode_mutex() {
    osMutexRelease(slot_mutex);
    osMutexRelease(tmode_mutex);
}

void srtt_terminate(uint32_t tid) {
    acquire_slot_tmode_mutex();
    
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
    if (res == 0) {
        set_slot_num(SLOT_S, 0);
    }
    set_slot_num(tid, 0);

    release_slot_tmode_mutex();
}

void change_srtt_to_hrtt(uint32_t tid) {
    acquire_slot_tmode_mutex();
    
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
    
    release_slot_tmode_mutex();
}

void change_hrtt_to_srtt(uint32_t tid) {
    acquire_slot_tmode_mutex();

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

    release_slot_tmode_mutex();
}