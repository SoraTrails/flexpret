#include "cmsis_os2.h"
#include "flexpret_threads.h"
#include "flexpret_test.h"
#include "flexpret_io.h"

typedef struct test_arg {
    int input;
    int output;
} test_arg;

void func (void* arg) {
    uint32_t tid = read_csr(hartid);
    test_arg* t = (test_arg*)arg;
    register int i;
    int tmp = 0;
    for (i = t->input; i < 8; i++) {
        tmp += (tmp + t->input - 1) + 2;
    }
    t->output = tmp;
}

int test_osThreadNew_osThreadJoin() {
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
    osThreadId_t tid1 = osThreadNew((osThreadFunc_t)func, (void*)(args + 0), &thread1);
    osThreadId_t tid2 = osThreadNew((osThreadFunc_t)func, (void*)(args + 1), &thread2);

    if (tid1 == NULL || tid2 == NULL) {
        FLEXPRET_TEST_FAILED();
        return FAILED;
    }
    osKernelStart();
    osThreadJoin(tid1);
    osThreadJoin(tid2);

    flexpret_info("Thread1 returns :");
    flexpret_info(itoa_hex(args[0].output));
    flexpret_info("\n");

    if (args[0].output != 124) {
        FLEXPRET_TEST_FAILED();
        return FAILED;
    }

    flexpret_info("Thread2 returns :");
    flexpret_info(itoa_hex(args[1].output));
    flexpret_info("\n");
    
    if (args[1].output != 42) {
        FLEXPRET_TEST_FAILED();
        return FAILED;
    }

    return PASSED;
}