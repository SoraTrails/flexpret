#include "flexpret_scheduler.h"
#include "flexpret_encoding.h"
#include "flexpret_threads.h"

extern volatile hwthread_state startup_state[FLEXPRET_HW_THREADS_NUMS];
extern osThreadAttr_t *flexpret_thread_attr_entry[FLEXPRET_HW_THREADS_NUMS];

int32_t osSchedulerGetFreq(osThreadId_t thread_id) {
    uint8_t slots[FLEXPRET_MAX_HW_THREADS_NUMS];
    get_slots(slots, slots + 1, slots + 2, slots + 3, slots + 4, slots + 5, slots + 6, slots + 7);
    register int i;
    uint8_t soft_count = 0;
    uint8_t active_count = 0;
    uint8_t thread_count = 0;
    uint32_t tmp = (uint32_t) thread_id;
    uint32_t tid = get_tid_by_offset(tmp - (uint32_t)startup_state);

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
    get_slots(slots, slots + 1, slots + 2, slots + 3, slots + 4, slots + 5, slots + 6, slots + 7);
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
    set_slots(slots[7], slots[6], slots[5], slots[4], slots[3], slots[2], slots[1], slots[0]);
    return osOK;
}

// TODO: It’s better to make the same thread's slots be interleaved
osStatus_t osSchedulerSetSlotNum(osThreadId_t thread_id, int count) {
    if (count < 0 || count >= FLEXPRET_MAX_HW_THREADS_NUMS - 1) {
        return osErrorParameter;
    }
    uint32_t tmp = (uint32_t) thread_id;
    uint32_t tid = get_tid_by_offset(tmp - (uint32_t)startup_state);

    return set_slot_num(tid, count);
}

osStatus_t osSchedulerSetSoftSlotNum(int count) {
    if (count < 0 || count >= FLEXPRET_MAX_HW_THREADS_NUMS - 1) {
        return osErrorParameter;
    }

    return set_slot_num(SLOT_S, count);
}

osStatus_t osSchedulerSetTmodes(osThreadId_t thread_id, int tmode) {
    uint32_t tmp = (uint32_t) thread_id;
    uint32_t tid = get_tid_by_offset(tmp - (uint32_t)startup_state);

    uint32_t tmodes[FLEXPRET_HW_THREADS_NUMS];
    get_tmodes_4(tmodes, tmodes + 1, tmodes + 2, tmodes + 3);
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

    set_tmodes_4(tmodes[3], tmodes[2], tmodes[1], tmodes[0]);
    return osOK;
}

int osSchedulerGetTmodes(osThreadId_t thread_id) {
    uint32_t tmp = (uint32_t) thread_id;
    uint32_t tid = get_tid_by_offset(tmp - (uint32_t)startup_state);

    uint32_t tmodes[FLEXPRET_HW_THREADS_NUMS];
    get_tmodes_4(tmodes, tmodes + 1, tmodes + 2, tmodes + 3);
    return tmodes[tid];
}

int32_t osSchedulerGetSRTTNum() {
    uint32_t tmodes[FLEXPRET_HW_THREADS_NUMS];
    get_tmodes_4(tmodes, tmodes + 1, tmodes + 2, tmodes + 3);
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
