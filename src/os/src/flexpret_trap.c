#include "flexpret_encoding.h"
#include "flexpret_const.h"
#include "flexpret_trap.h"
#include "flexpret_threads.h"
#include "flexpret_io.h"
#include "flexpret_utils.h"

// trap handler vector, defined in trap.S
extern volatile trap_vector trap_handler_vector[FLEXPRET_HW_THREADS_NUMS];

void default_ie_handler() {
    uint32_t tid = read_csr(hartid);
    flexpret_info("Thread ");
    flexpret_info(itoa_hex_removing_ldz(tid));
    flexpret_info(" trapped in ie.\n");
}

void default_ee_handler() {
    uint32_t tid = read_csr(hartid);
    flexpret_info("Thread ");
    flexpret_info(itoa_hex_removing_ldz(tid));
    flexpret_info(" trapped in ee.\n");
}

void default_int_handler() {
    uint32_t tid = read_csr(hartid);
    flexpret_info("Thread ");
    flexpret_info(itoa_hex_removing_ldz(tid));
    flexpret_info(" got an external interrupt.\n");
}

void default_fault_handler() {
    uint32_t tid = read_csr(hartid);
    flexpret_error("Thread ");
    flexpret_error(itoa_hex_removing_ldz(tid));
    flexpret_error(" got an internal fault, cause is: ");
    uint32_t cause = read_csr(cause);
    flexpret_error(itoa_hex(cause));
    flexpret_error("\n");
}


osStatus_t osKernelSetTrapHandler(osThreadId_t thread_id, void (*handler)(), osTrapType_t t) {
    if (handler == NULL) {
        flexpret_error("Bad trap handler\n");
        return osError;
    }
    if (t < 0 || t >= TRAP_NUM_MAX) {
        flexpret_error("Bad trap type\n");
        return osError;
    }
    uint32_t tid = thread_id == NULL ? read_csr(hartid) : get_tid(thread_id);
    trap_handler_vector[tid].handler[t] = (trap_handler)handler;
    return osOK;
}

trap_handler osKernelGetTrapHandler(osThreadId_t thread_id, osTrapType_t t) {
    if (t < 0 || t >= TRAP_NUM_MAX) {
        flexpret_error("Bad trap type\n");
        return NULL;
    }
    uint32_t tid = thread_id == NULL ? read_csr(hartid) : get_tid(thread_id);

    return trap_handler_vector[tid].handler[t];
}