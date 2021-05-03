#include "cmsis_os2.h"
#include "flexpret_os.h"
#include "flexpret_test.h"

static void test_func(void *arg) {
    return;
}

int test_api_cycle() {
    osThreadAttr_t thread;
    thread.priority = osPriorityRealtime;
    thread.attr_bits = osThreadJoinable;

    // osKernelInitialize
    osKernelInitialize();
    
    // osThreadNew
    osThreadId_t t1 = osThreadNew(test_func, NULL, NULL);
    osThreadId_t t2 = osThreadNew(test_func, NULL, &thread);

    // osKernelStart
    osKernelStart();

    // osThreadGetState
    osThreadGetState(t1);

    // osThreadSetPriority
    osThreadSetPriority(t1, osPriorityRealtime);
    osThreadSetPriority(t2, osPriorityNormal);

    // osThreadGetPriority
    osThreadGetPriority(t1);
    
    // osThreadSuspend
    osThreadSuspend(t2);

    // osThreadResume
    osThreadResume(t2);

    // osTimerNew
    osTimerId_t timer = osTimerNew(test_func, osTimerPeriodic, NULL, NULL);

    // osTimerStart
    osTimerStart(timer, 10000);
    
    // osTimerStop
    osTimerStop(timer);


    const osMutexAttr_t threadMutexAttr = {
        "myThreadMutex",     // human readable mutex name
        osMutexRecursive | osMutexRobust,    // attr_bits
        NULL,                // memory for control block   
        0U                   // size for control block
    };
    osMutexId_t mutex = osMutexNew(&threadMutexAttr);

    osMutexAcquire(mutex, osWaitForever);

    // osMutexRelease
    osMutexRelease(mutex);

    return PASSED;
}
