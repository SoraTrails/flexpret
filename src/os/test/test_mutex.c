#include "cmsis_os2.h"
#include "flexpret_const.h"
#include "flexpret_test.h"
#include "flexpret_utils.h"
#include "flexpret_io.h"
#include "flexpret_mutex.h"

osMutexId_t mutex;
int sum1 = 0;
int sum2 = 0;

const osMutexAttr_t threadMutexAttr = {
    "myThreadMutex",     // human readable mutex name
    osMutexRecursive | osMutexRobust,    // attr_bits
    NULL,                // memory for control block   
    0U                   // size for control block
};

static void osTestMutex(void * arg) {
    int *p_arg = (int*) arg;
    register int i;

    for (i = 0; i < 3; i++) {
        int tmp = sum1;
        register int j;
        for (j = 0; j < 3; j++) {
            tmp += *p_arg;
        }
        sum1 = tmp;
    }
    
    for (i = 0; i < 3; i++) {
        // while (osMutexAcquire(mutex, 0) != osOK)
        //     ;
        osMutexAcquire(mutex, osWaitForever);
        int tmp = sum2;
        register int j;
        for (j = 0; j < 3; j++) {
            tmp += *p_arg;
        }
        sum2 = tmp;
        osMutexRelease(mutex);
    }
}

int test_mutex() {
    osThreadAttr_t thread1;
    thread1.priority = osPriorityRealtime;
    thread1.attr_bits = osThreadJoinable;
    osThreadAttr_t thread2;
    thread2.priority = osPriorityRealtime;
    thread2.attr_bits = osThreadJoinable;
    osThreadAttr_t thread3;
    thread3.priority = osPriorityRealtime;
    thread3.attr_bits = osThreadJoinable;
    osKernelInitialize();

    osThreadId_t tid1, tid2, tid3; 
    int arg1 = 2;
    int arg2 = 5;
    int arg3 = 6;
    mutex = osMutexNew(&threadMutexAttr);
    FLEXPRET_ASSERT(mutex != NULL);

    tid1 = osThreadNew((osThreadFunc_t)osTestMutex, &arg1, &thread1);
    tid2 = osThreadNew((osThreadFunc_t)osTestMutex, &arg2, &thread2);
    tid3 = osThreadNew((osThreadFunc_t)osTestMutex, &arg3, &thread3);

    FLEXPRET_ASSERT(tid1 != NULL && tid2 != NULL  && tid3 != NULL);

    osKernelStart();

    osThreadJoin(tid1);
    osThreadJoin(tid2);
    osThreadJoin(tid3);

    flexpret_info("Result: sum1: ");
    flexpret_info(itoa_hex_removing_ldz(sum1));
    flexpret_info(", sum2: ");
    flexpret_info(itoa_hex_removing_ldz(sum2));
    flexpret_info(".\n");
    FLEXPRET_ASSERT(sum2 == 117); // (2 + 5 + 6) * 9
    
    return PASSED;
}

int test_mutex_spin() { // TC_MutexSpin
    osMutexId_t m;
    const osMutexAttr_t mAttr = {
        "myThreadMutex",     // human readable mutex name
        osMutexRecursive | osMutexRobust,    // attr_bits
        NULL,                // memory for control block   
        0U                   // size for control block
    };

    m = osMutexNew(&mAttr);
    FLEXPRET_ASSERT(m != NULL);
    FLEXPRET_ASSERT(osMutexSetSpin(m, 1) == osOK);
    FLEXPRET_ASSERT(osMutexGetSpin(m) == 1);
    return PASSED;
}