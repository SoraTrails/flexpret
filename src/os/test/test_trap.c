#include "cmsis_os2.h"
#include "flexpret_const.h"
#include "flexpret_test.h"
#include "flexpret_utils.h"
#include "flexpret_io.h"
#include "flexpret_trap.h"

static void osTestFault(void * arg) {
    int arr[10] = {2, 1, 5, 7, 9, 10, 6, 2, 8, 5};
    register int i, j;
    for (i = 0; i < 10; i++) {
        for (j = i + 1; j < 10; j++) {
            if (arr[i] < arr[j]) {
                int tmp = arr[i];
                arr[i] = arr[j];
                arr[j] = tmp;
                tmp /= 97;  // unknown instruction fault
            }
        } 
    }
    flexpret_info("Array: ");
    for (i = 0; i < 10; i++) {
        flexpret_info(itoa_hex_removing_ldz(arr[i]));
    }
    flexpret_info("\n");
}

static void fault_handler() {
    flexpret_info("fault!\n");
}

int test_trap() {
    osThreadAttr_t thread;
    thread.priority = osPriorityNormal;
    thread.attr_bits = osThreadJoinable;
    osKernelInitialize();
    osThreadId_t tid = osThreadNew((osThreadFunc_t)osTestFault, NULL, &thread);
    FLEXPRET_ASSERT(tid != NULL);

    osKernelStart();
    FLEXPRET_ASSERT(osThreadSetTrapHandler(tid, (trap_handler)fault_handler, Exception) == osOK);

    FLEXPRET_ASSERT(osThreadGetTrapHandler(tid, InterruptOnExpire) == (trap_handler)default_ie_handler);
    FLEXPRET_ASSERT(osThreadGetTrapHandler(tid, ExceptionOnExpire) == (trap_handler)default_ee_handler);
    FLEXPRET_ASSERT(osThreadGetTrapHandler(tid, ExternalInterrupt) == (trap_handler)default_int_handler);

    FLEXPRET_ASSERT(osThreadGetTrapHandler(tid, Exception) == (trap_handler)fault_handler);

    osThreadJoin(tid);
    
    return PASSED;
}
