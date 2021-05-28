#include "cmsis_os2.h"
#include "flexpret_os.h"
#include "RV_Framework.h"

#define FLEXPRET_ASSERT(...) ASSERT_TRUE(__VA_ARGS__)
#define FLEXPRET_DEBUG_SLOT_TMODE()

#ifdef RTE_RV_EXTEND
#define flexpret_info(...)  
// #define flexpret_error(...) 
#define flexpret_warn(...) 
#define PASSED
#endif

typedef struct test_arg {
    int input;
    int output;
} test_arg;

static void osTestThread1 (void* arg) {
    uint32_t tid = read_csr(hartid);
    test_arg* t = (test_arg*)arg;
    // if (t->input == 3) {
    //     while (1) {}
    // }
    register int i;
    int tmp = 0;
    for (i = t->input; i < 8; i++) {
        tmp += (tmp + t->input - 1) + 2;
    }
    t->output = tmp;
}

#ifdef RTE_RV_EXTEND
void TC_ThreadJoin() {
#else
int test_osThreadNew_osThreadJoin() { // TC_ThreadJoin
#endif
	const osThreadAttr_t attr[2] = {
		{ "Th_Thr0", osThreadJoinable, NULL, sizeof(hwthread_state), NULL, 0, osPriorityRealtime, 0, 0},
		{ "Th_Thr1", osThreadJoinable, NULL, sizeof(hwthread_state), NULL, 0, osPriorityNormal, 0, 0},
    };
    test_arg args[2];
    args[0].input = 3;
    args[1].input = 5;
    args[0].output = -1;
    args[1].output = -1;

    // osKernelInitialize();
    osThreadId_t tid1 = osThreadNew((osThreadFunc_t)osTestThread1, (void*)(args + 0), &attr[0]);
    osThreadId_t tid2 = osThreadNew((osThreadFunc_t)osTestThread1, (void*)(args + 1), &attr[1]);

    FLEXPRET_ASSERT(tid1 != NULL && tid2 != NULL);

    // osKernelStart();
    osThreadJoin(tid1);
    osThreadJoin(tid2);

    flexpret_info("Thread1 returns :");
    flexpret_info(itoa_hex(args[0].output));
    flexpret_info("\n");

    FLEXPRET_ASSERT(args[0].output == 124);
    
    flexpret_info("Thread2 returns :");
    flexpret_info(itoa_hex(args[1].output));
    flexpret_info("\n");
    
    FLEXPRET_ASSERT(args[1].output == 42);

    return PASSED;
}

static void osTestThread3 (void* arg) {
    osThreadId_t tid = osThreadGetId();
    register int i;
    for (i = 0; i < 100; i++) {}
    // osThreadExit();
}

static void osTestThread4 (void* arg) {
    osThreadId_t tid = osThreadGetId();
    register int i;
    for (i = 0; i < 100; i++) {}
    // for (;;) {}
}

#ifdef RTE_RV_EXTEND
void TC_ThreadControl() {
#else
int test_osThread_control() { // TC_ThreadControl
#endif
	const osThreadAttr_t attr[3] = {
		{ "thread1", osThreadJoinable, NULL, sizeof(hwthread_state), NULL, 0, osPriorityRealtime, 0, 0},
		{ "thread2", osThreadJoinable, NULL, sizeof(hwthread_state), NULL, 0, osPriorityNormal, 0, 0},
		{ "thread3", osThreadJoinable, NULL, sizeof(hwthread_state), NULL, 0, osPriorityNormal, 0, 0},
    };

    // osKernelInitialize();
    osThreadId_t tids[3];
    tids[0] = osThreadNew((osThreadFunc_t)osTestThread3, NULL, &attr[0]);
    tids[1] = osThreadNew((osThreadFunc_t)osTestThread4, NULL, &attr[1]);
    tids[2] = osThreadNew((osThreadFunc_t)osTestThread4, NULL, &attr[2]);

    FLEXPRET_ASSERT(tids[0] != NULL && tids[1] != NULL && tids[2] != NULL);

    // osKernelStart();
    const int delay_period = 1500;
    FLEXPRET_DEBUG_SLOT_TMODE();
    osDelay(delay_period);
    FLEXPRET_ASSERT(osOK == osThreadSetPriority(tids[0], osPriorityNormal));
    FLEXPRET_DEBUG_SLOT_TMODE();
    FLEXPRET_ASSERT(osOK == osThreadSetPriority(tids[1], osPriorityRealtime));
    FLEXPRET_DEBUG_SLOT_TMODE();
    osDelay(delay_period);

    uint32_t count = osThreadGetCount();
    flexpret_info("mainThread: 1. active thread count: ");
    flexpret_info(itoa_hex_removing_ldz(count));
    flexpret_info("\n");
    osPriority_t prior = osThreadGetPriority(tids[0]);
    FLEXPRET_ASSERT(prior == osPriorityNormal);
    prior = osThreadGetPriority(tids[1]);
    FLEXPRET_ASSERT(prior == osPriorityRealtime);
    FLEXPRET_ASSERT(osOK == osThreadSuspend(tids[1]));
    FLEXPRET_DEBUG_SLOT_TMODE();
    osDelay(delay_period);
    FLEXPRET_ASSERT(osOK == osThreadResume(tids[1]));
    FLEXPRET_DEBUG_SLOT_TMODE();
    osDelay(delay_period);
    // FLEXPRET_ASSERT(osOK == osThreadDetach(tids[1]));
    uint32_t tick = osKernelGetTickCount();
    osDelayUntil(tick + delay_period);
    FLEXPRET_ASSERT(osOK == osThreadJoin(tids[0]));
    FLEXPRET_ASSERT(osOK == osThreadJoin(tids[1]));
    FLEXPRET_ASSERT(osOK == osThreadJoin(tids[2]));
    // FLEXPRET_ASSERT(osOK == osThreadTerminate(tids[1]));
    // FLEXPRET_DEBUG_SLOT_TMODE();
    // FLEXPRET_ASSERT(osOK == osThreadTerminate(tids[2]));
    // FLEXPRET_DEBUG_SLOT_TMODE();

    osThreadId_t thread_array[4];
    uint32_t array_items = 4;
    count = osThreadEnumerate(thread_array, array_items);
    flexpret_info("mainThread: 2. active thread count: ");
    flexpret_info(itoa_hex_removing_ldz(count));
    flexpret_info("\n");

    return PASSED;
} 

static int fault_flag = 0;

static void osTestFault(void * arg) {
    int arr[10] = {2, 1, 5, 7, 9, 10, 6, 2, 8, 5};
    register int i, j;
    for (i = 0; i < 10; i++) {
        for (j = i + 1; j < 10; j++) {
            if (arr[i] < arr[j]) {
                int tmp = arr[i];
                arr[i] = arr[j];
                arr[j] = tmp;
                asm volatile ("div a0, a1, a2"); // unknown instruction fault
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
    fault_flag = 0xdead;
}

#ifdef RTE_RV_EXTEND
void TC_ThreadTrap() {
#else
int test_trap() { // TC_ThreadTrap
#endif
    osThreadAttr_t thread;
    thread.priority = osPriorityNormal;
    thread.attr_bits = osThreadJoinable;
    // osKernelInitialize();
    osThreadId_t tid = osThreadNew((osThreadFunc_t)osTestFault, NULL, &thread);
    FLEXPRET_ASSERT(tid != NULL);

    // osKernelStart();
    FLEXPRET_ASSERT(osThreadSetTrapHandler(tid, (trap_handler)fault_handler, Exception) == osOK);

    FLEXPRET_ASSERT(osThreadGetTrapHandler(tid, InterruptOnExpire) == (trap_handler)default_ie_handler);
    FLEXPRET_ASSERT(osThreadGetTrapHandler(tid, ExceptionOnExpire) == (trap_handler)default_ee_handler);
    FLEXPRET_ASSERT(osThreadGetTrapHandler(tid, ExternalInterrupt) == (trap_handler)default_int_handler);

    FLEXPRET_ASSERT(osThreadGetTrapHandler(tid, Exception) == (trap_handler)fault_handler);

    osThreadJoin(tid);

    FLEXPRET_ASSERT(fault_flag == 0xdead);
    flexpret_error(itoa_hex_removing_ldz(fault_flag));
    flexpret_error("\n");

    return PASSED;
}


#ifdef RTE_RV_EXTEND
void TC_MutexSpin() {
#else
int test_mutex_spin() { // TC_MutexSpin
#endif
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


static void osTestThread5 (void* arg) {
    osThreadId_t tid = osThreadGetId();
    register int i;
    for (i = 0; i < 1000; i++) {}
    // for (;;) {}
}

#ifdef RTE_RV_EXTEND
void TC_SchedulerSlot() {
#else
int test_scheduler_slot() { // TC_SchedulerSlot
#endif
	const osThreadAttr_t attr[1] = {
		{ "thread1", osThreadJoinable, NULL, sizeof(hwthread_state), NULL, 0, osPriorityNormal, 0, 0},
    };

    // osKernelInitialize();
    osThreadId_t tid = osThreadNew((osThreadFunc_t)osTestThread5, NULL, &attr[0]);
    // osKernelStart();
    FLEXPRET_ASSERT(osSchedulerSetSlotNum(tid, 1) == osOK);
    FLEXPRET_ASSERT(osSchedulerSetSoftSlotNum(0) == osOK);
    FLEXPRET_ASSERT(osSchedulerGetSRTTNum() == 1);
    int num = osSchedulerGetFreq(tid);
    FLEXPRET_ASSERT(THREAD_FREQ(num) == 1);
    FLEXPRET_ASSERT(SOFT_SLOT_COUNT(num) == 0);
    FLEXPRET_ASSERT(ALL_SLOT_COUNT(num) == 2);

    osThreadJoin(tid);
    return PASSED;
}


#ifdef RTE_RV_EXTEND
void TC_SchedulerTmode() {
#else
int test_scheduler_tmode() { // TC_SchedulerTmode
#endif
	const osThreadAttr_t attr[1] = {
		{ "thread1", osThreadJoinable, NULL, sizeof(hwthread_state), NULL, 0, osPriorityNormal, 0, 0},
    };
    // osKernelInitialize();
    osThreadId_t tid = osThreadNew((osThreadFunc_t)osTestThread5, NULL, &attr[0]);
    // osKernelStart();
    FLEXPRET_ASSERT(osSchedulerSetTmodes(tid, TMODE_HA) == osOK);
    FLEXPRET_ASSERT(osSchedulerGetTmodes(tid) == TMODE_HA);
    FLEXPRET_ASSERT(osSchedulerSetTmodes(tid, TMODE_SA) == osOK);
    FLEXPRET_ASSERT(osSchedulerGetSRTTNum() == 1);

    osThreadJoin(tid);
    return PASSED;
}