#include "cmsis_os2.h"
#include "flexpret_mutex.h"
#include "flexpret_utils.h"
#include "flexpret_threads.h"
#include "flexpret_timing.h"

static int flexpret_mutex_num = 0;
volatile mutex_state startup_mutex_state[FLEXPRET_MUTEX_MAX_NUM];

void write_mutex_csr(int i, int val) {
    switch (i)
    {
        case 0:write_csr_marco(CSR_MUTEX_0, val);return;
        case 1:write_csr_marco(CSR_MUTEX_1, val);return;
        case 2:write_csr_marco(CSR_MUTEX_2, val);return;
        case 3:write_csr_marco(CSR_MUTEX_3, val);return;
        case 4:write_csr_marco(CSR_MUTEX_4, val);return;
        case 5:write_csr_marco(CSR_MUTEX_5, val);return;
        case 6:write_csr_marco(CSR_MUTEX_6, val);return;
        case 7:write_csr_marco(CSR_MUTEX_7, val);return;
        default:return;
    }
}

int swap_mutex_csr(int i, int val) {
    switch (i)
    {
        case 0:return swap_csr_marco(CSR_MUTEX_0, val);
        case 1:return swap_csr_marco(CSR_MUTEX_1, val);
        case 2:return swap_csr_marco(CSR_MUTEX_2, val);
        case 3:return swap_csr_marco(CSR_MUTEX_3, val);
        case 4:return swap_csr_marco(CSR_MUTEX_4, val);
        case 5:return swap_csr_marco(CSR_MUTEX_5, val);
        case 6:return swap_csr_marco(CSR_MUTEX_6, val);
        case 7:return swap_csr_marco(CSR_MUTEX_7, val);
        default: return -1;
    }
}

//  ==== Mutex Management Functions ====
osMutexId_t osMutexNew (const osMutexAttr_t *attr) {
    if (attr == NULL) {
        flexpret_error("Dynamic memory alloc of mutex is currently not supported, plz use static alloc instead.\n");
        return NULL;
    }
    if (flexpret_mutex_num >= FLEXPRET_MUTEX_MAX_NUM) {
        flexpret_error("Max mutex num exceed\n");
        return NULL;
    }
    if (attr->attr_bits & osMutexPrioInherit) {
        flexpret_error("osMutexPrioInherit attr is not support.\n");
    }
    int mid = flexpret_mutex_num++;
    volatile mutex_state* mutex = &startup_mutex_state[mid];

    mutex->csr_addr = mid;
    mutex->ifRecursive = attr->attr_bits & osMutexRecursive;
    mutex->ifRobust = attr->attr_bits & osMutexRobust;
    mutex->owner = NULL;
    mutex->name = attr->name;
    mutex->active = 1;
    mutex->count = 0;
    write_mutex_csr(mid, FLEXPRET_MUTEX_ACTIVE);

    return (osMutexId_t)mutex;
}

const char *osMutexGetName (osMutexId_t mutex_id) {
    mutex_state* mutex = (mutex_state*) mutex_id;
    if (mutex != NULL)
        return mutex->name;
    return "";
}

osStatus_t osMutexAcquire (osMutexId_t mutex_id, uint32_t timeout) {
    if (mutex_id == NULL) {
        return osErrorParameter;
    }
    mutex_state* mutex = (mutex_state*) mutex_id;
    if (mutex->active == 0) {
        // mutex has been deleted
        return osErrorParameter;
    }
    if (swap_mutex_csr(mutex->csr_addr, FLEXPRET_MUTEX_INACTIVE) == FLEXPRET_MUTEX_ACTIVE) {
        // succeed acquire the mutex
        if (mutex->owner == NULL) {
            mutex->owner = osThreadGetId();
            mutex->count = 1;
            return osOK;
        } else {
            // has owner but mutex can be acquired
            flexpret_error("Bad mutex status\n");
            write_mutex_csr(mutex->csr_addr, FLEXPRET_MUTEX_ACTIVE);
            return osError;
        }
    } else {
        // failed to acquire the mutex
        if (mutex->owner == osThreadGetId() && mutex->ifRecursive) {
            mutex->count++;
            return osOK;
        } else {
            if (timeout == 0) {
                return osErrorResource;
            }
            // TODO: add spin lock wait mechanism
            if (timeout == osWaitForever) {
                while (1) {
                    if (swap_mutex_csr(mutex->csr_addr, FLEXPRET_MUTEX_INACTIVE) == FLEXPRET_MUTEX_ACTIVE) {
                        break;
                    }
                    osDelay(FLEXPRET_WAIT_PERIOD);
                }
                mutex->owner = osThreadGetId();
                mutex->count = 1;
                return osOK;
            } else {
                register uint32_t time_period = timeout > FLEXPRET_WAIT_PERIOD ? FLEXPRET_WAIT_PERIOD : timeout;
                register uint32_t all_time = 0;
                do {
                    osDelay(time_period);
                    if (swap_mutex_csr(mutex->csr_addr, FLEXPRET_MUTEX_INACTIVE) == FLEXPRET_MUTEX_ACTIVE) {
                        mutex->owner = osThreadGetId();
                        mutex->count = 1;
                        return osOK;
                    }
                    all_time += time_period;
                } while (all_time < timeout);

                return osErrorTimeout;
            }
        }
    }
    return osError;
}

osStatus_t osMutexRelease (osMutexId_t mutex_id) {
    if (mutex_id == NULL) {
        return osErrorParameter;
    }
    mutex_state* mutex = (mutex_state*) mutex_id;
    if (mutex->active == 0) {
        // mutex has been deleted
        return osErrorParameter;
    }
    if (mutex->owner == NULL || mutex->owner != osThreadGetId()) {
        return osErrorResource;
    }
    mutex->count--;
    if (mutex->count == 0) {
        mutex->owner = NULL;
        write_mutex_csr(mutex->csr_addr, FLEXPRET_MUTEX_ACTIVE);
    }
    return osOK;
}

osThreadId_t osMutexGetOwner (osMutexId_t mutex_id) {
    if (mutex_id == NULL) {
        return NULL;
    }
    mutex_state* mutex = (mutex_state*) mutex_id;
    if (mutex->active == 0) {
        // mutex has been deleted
        return NULL;
    }
    return mutex->owner;
}

osStatus_t osMutexDelete (osMutexId_t mutex_id) {
    if (mutex_id == NULL) {
        return osErrorParameter;
    }
    mutex_state* mutex = (mutex_state*) mutex_id;
    if (mutex->active == 0) {
        // mutex has been deleted
        return osErrorParameter;
    }
    mutex->active = 0;
    // TODO : delete an active mutex should return busy Error.
    return osOK;
}

 