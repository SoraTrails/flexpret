#include "cmsis_os2.h"
#include "flexpret_threads.h"
#include "flexpret_test.h"
#include "flexpret_io.h"

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

int test_osThreadNew_osThreadJoin() { // TC_ThreadJoin
    osThreadAttr_t thread1;
    osThreadAttr_t thread2;
    thread1.priority = osPriorityRealtime;
    thread1.attr_bits = osThreadJoinable;
    thread2.priority = osPriorityNormal;
    thread2.attr_bits = osThreadJoinable;
    test_arg args[2];
    args[0].input = 3;
    args[1].input = 5;
    args[0].output = -1;
    args[1].output = -1;

    osKernelInitialize();
    osThreadId_t tid1 = osThreadNew((osThreadFunc_t)osTestThread1, (void*)(args + 0), &thread1);
    osThreadId_t tid2 = osThreadNew((osThreadFunc_t)osTestThread1, (void*)(args + 1), &thread2);

    FLEXPRET_ASSERT(tid1 != NULL && tid2 != NULL);

    osKernelStart();
    osThreadJoin(tid2);
    osThreadJoin(tid1);

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

static void osTestThread2 (void* arg) {
    if (arg == NULL) {
        flexpret_info("osTestThread: arg is NULL\n");
    } else {
        flexpret_info("osTestThread: arg is ");
        flexpret_info(itoa_hex_removing_ldz((uint32_t)arg));
        flexpret_info("\n");
    }

    osThreadId_t tid = osThreadGetId();
    flexpret_info("osTestThread: tid is ");
    flexpret_info(itoa_hex((uint32_t)tid));
    flexpret_info("\nosTestThread: stackSize is ");
    flexpret_info(itoa_hex_removing_ldz(osThreadGetStackSize(tid)));
    flexpret_info("\nosTestThread: stackSpace is ");
    flexpret_info(itoa_hex_removing_ldz(osThreadGetStackSpace(tid)));
    flexpret_info("\n");
}


int test_osThread_info() {
    osThreadAttr_t thread1;
    thread1.name = "test info thread";
    thread1.priority = osPriorityRealtime;
    thread1.attr_bits = osThreadJoinable;

    osKernelInitialize();
    osThreadId_t tid1 = osThreadNew((osThreadFunc_t)osTestThread2, NULL, &thread1);

    FLEXPRET_ASSERT(tid1 != NULL);

    osThreadId_t tid0 = osThreadGetId();
    flexpret_info("mainThread: sub thread's name is ");
    flexpret_info(osThreadGetName(tid1));
    flexpret_info("\nmainThread: sub thread's state is ");
    flexpret_info(itoa_hex_removing_ldz(osThreadGetState(tid1)));
    flexpret_info("\nmainThread: stackSize is ");
    flexpret_info(itoa_hex_removing_ldz(osThreadGetStackSize(tid0)));
    flexpret_info("\nmainThread: stackSpace is ");
    flexpret_info(itoa_hex_removing_ldz(osThreadGetStackSpace(tid0)));
    flexpret_info("\n");

    osKernelStart();

    osThreadJoin(tid1);

    return PASSED;
}

static void osTestThread3 (void* arg) {
    osThreadId_t tid = osThreadGetId();
    register int i;
    for (i = 0; i < 100; i++) {}
    osThreadExit();
}

static void osTestThread4 (void* arg) {
    osThreadId_t tid = osThreadGetId();
    for (;;) {}
}

int test_osThread_control() { // TC_ThreadControl
    osThreadAttr_t thread1;
    thread1.name = "test control thread1";
    thread1.priority = osPriorityRealtime;
    thread1.attr_bits = osThreadJoinable;
    osThreadAttr_t thread2;
    thread2.name = "test control thread2";
    thread2.priority = osPriorityNormal;
    thread2.attr_bits = osThreadJoinable;

    osKernelInitialize();
    osThreadId_t tid1 = osThreadNew((osThreadFunc_t)osTestThread3, NULL, &thread1);
    osThreadId_t tid2 = osThreadNew((osThreadFunc_t)osTestThread4, NULL, NULL);
    osThreadId_t tid3 = osThreadNew((osThreadFunc_t)osTestThread4, NULL, &thread2);

    FLEXPRET_ASSERT(tid1 != NULL && tid2 != NULL && tid3 != NULL);

    osKernelStart();
    const int delay_period = 1500;
    FLEXPRET_DEBUG_SLOT_TMODE();
    osDelay(delay_period);
    FLEXPRET_ASSERT(osOK == osThreadSetPriority(tid1, osPriorityNormal));
    FLEXPRET_DEBUG_SLOT_TMODE();
    FLEXPRET_ASSERT(osOK == osThreadSetPriority(tid2, osPriorityRealtime));
    FLEXPRET_DEBUG_SLOT_TMODE();
    osDelay(delay_period);

    uint32_t count = osThreadGetCount();
    flexpret_info("mainThread: 1. active thread count: ");
    flexpret_info(itoa_hex_removing_ldz(count));
    flexpret_info("\n");
    osPriority_t prior = osThreadGetPriority(tid1);
    FLEXPRET_ASSERT(prior == osPriorityNormal)
    prior = osThreadGetPriority(tid2);
    FLEXPRET_ASSERT(prior == osPriorityRealtime)
    FLEXPRET_ASSERT(osOK == osThreadSuspend(tid2));
    FLEXPRET_DEBUG_SLOT_TMODE();
    osDelay(delay_period);
    FLEXPRET_ASSERT(osOK == osThreadResume(tid2));
    FLEXPRET_DEBUG_SLOT_TMODE();
    osDelay(delay_period);
    FLEXPRET_ASSERT(osOK == osThreadDetach(tid2));
    uint32_t tick = osKernelGetTickCount();
    osDelayUntil(tick + delay_period);
    FLEXPRET_ASSERT(osOK != osThreadJoin(tid2));
    FLEXPRET_ASSERT(osOK == osThreadTerminate(tid2));
    FLEXPRET_DEBUG_SLOT_TMODE();
    FLEXPRET_ASSERT(osOK == osThreadTerminate(tid3));
    FLEXPRET_DEBUG_SLOT_TMODE();

    osThreadId_t thread_array[4];
    uint32_t array_items = 4;
    count = osThreadEnumerate(thread_array, array_items);
    flexpret_info("mainThread: 2. active thread count: ");
    flexpret_info(itoa_hex_removing_ldz(count));
    flexpret_info("\n");

    return PASSED;
} 