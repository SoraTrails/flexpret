#include "cmsis_os2.h"
#include "flexpret_const.h"
#include "flexpret_test.h"
#include "flexpret_utils.h"
#include "flexpret_io.h"
#include "flexpret_timing.h"

static void timer_func(void * arg) {
    static int i = 0;
    int *t = (int*) arg;
    flexpret_info("Timer: arg: ");
    flexpret_info(itoa_hex_removing_ldz(*t));
    flexpret_info(", count: ");
    flexpret_info(itoa_hex_removing_ldz(i++));
    flexpret_info("\n");
}

static void osTestTimer1(void * arg) {
    int t = 11;
    osTimerId_t timer1 = osTimerNew(timer_func, osTimerPeriodic, &t, NULL);
    FLEXPRET_THREAD_ASSERT(osTimerStart(timer1, 4000) == osOK);
    flexpret_info(osTimerGetName(timer1));
    flexpret_info("\n");

    FLEXPRET_THREAD_ASSERT(osTimerIsRunning(timer1));

    // register int i;
    // for (i = 0; i < 1000; i++) {}

    FLEXPRET_THREAD_ASSERT(osTimerStop(timer1) == osOK);

    FLEXPRET_THREAD_ASSERT(!osTimerIsRunning(timer1));
    FLEXPRET_THREAD_ASSERT(osTimerDelete(timer1) == osOK);
}

static void osTestTimer2(void * arg) {
    int t = 7;
    osTimerId_t timer2 = osTimerNew(timer_func, osTimerPeriodic, &t, NULL);
    FLEXPRET_THREAD_ASSERT(osTimerStart(timer2, 4000) == osOK);
    flexpret_info(osTimerGetName(timer2));
    flexpret_info("\n");

    // register int i;
    // for (i = 0; i < 100; i++) {}

    // FLEXPRET_THREAD_ASSERT(!osTimerIsRunning(timer2));
    // FLEXPRET_THREAD_ASSERT(osTimerDelete(timer2) == osOK);
    FLEXPRET_THREAD_ASSERT(osTimerIsRunning(timer2));

    FLEXPRET_THREAD_ASSERT(osTimerStop(timer2) == osOK);

    FLEXPRET_THREAD_ASSERT(!osTimerIsRunning(timer2));
    FLEXPRET_THREAD_ASSERT(osTimerDelete(timer2) == osOK);
}

int test_timer() {
    osThreadAttr_t thread1;
    thread1.priority = osPriorityRealtime;
    thread1.attr_bits = osThreadJoinable;
    osThreadAttr_t thread2;
    thread2.priority = osPriorityRealtime;
    thread2.attr_bits = osThreadJoinable;
    osKernelInitialize();

    int t = 13;
    osTimerId_t timer = osTimerNew(timer_func, osTimerPeriodic, &t, NULL);
    FLEXPRET_ASSERT(osTimerStart(timer, 4000) == osOK);

    osThreadId_t tid1 = osThreadNew((osThreadFunc_t)osTestTimer1, NULL, &thread1);
    osThreadId_t tid2 = osThreadNew((osThreadFunc_t)osTestTimer2, NULL, &thread2);

    FLEXPRET_ASSERT(tid1 != NULL && tid2 != NULL);

    osKernelStart();


    osThreadJoin(tid1);
    osThreadJoin(tid2);
    FLEXPRET_ASSERT(osTimerDelete(timer) == osOK);
    
    return PASSED;
}

static void osTestMtfdImm() {
    mt_imm(1000);
    register int i;
    for (i = 0; i < 100; i++) {}
    fd();
}

static void osTestMtfdReg() {
    register int a = 100000;
    mt_reg(a);
    register int i;
    for (i = 0; i < 100; i++) {}
    fd();
}

int test_mtfd() {
    osKernelInitialize();
    osThreadAttr_t thread1;
    thread1.priority = osPriorityRealtime;
    thread1.attr_bits = osThreadJoinable;
    osThreadAttr_t thread2;
    thread2.priority = osPriorityRealtime;
    thread2.attr_bits = osThreadJoinable;
    osThreadId_t tid1 = osThreadNew((osThreadFunc_t)osTestMtfdImm, NULL, &thread1);
    osThreadId_t tid2 = osThreadNew((osThreadFunc_t)osTestMtfdReg, NULL, &thread2);

    FLEXPRET_ASSERT(tid1 != NULL && tid2 != NULL);

    osKernelStart();

    osThreadJoin(tid1);
    osThreadJoin(tid2);

    return PASSED;
}
