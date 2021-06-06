#include "cmsis_os2.h"
#include "flexpret_const.h"
#include "flexpret_test.h"
#include "flexpret_utils.h"
#include "flexpret_io.h"
#include "flexpret_timing.h"
#include "flexpret_scheduler.h"
#include "flexpret_threads.h"
#include "flexpret_trap.h"

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
    FLEXPRET_THREAD_ASSERT(osTimerStart(timer1, 10000) == osOK);
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
    FLEXPRET_THREAD_ASSERT(osTimerStart(timer2, 10000) == osOK);
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
    FLEXPRET_ASSERT(osTimerStart(timer, 10000) == osOK);

    osThreadId_t tid1 = osThreadNew((osThreadFunc_t)osTestTimer1, NULL, &thread1);
    osThreadId_t tid2 = osThreadNew((osThreadFunc_t)osTestTimer2, NULL, &thread2);

    FLEXPRET_ASSERT(tid1 != NULL && tid2 != NULL);

    osKernelStart();


    osThreadJoin(tid1);
    osThreadJoin(tid2);
    FLEXPRET_ASSERT(osTimerDelete(timer) == osOK);
    
    return PASSED;
}

void osTestTimerSuspend(void *arg) {
    int t = 15;
    osTimerId_t timer = osTimerNew(timer_func, osTimerPeriodic, &t, NULL);
    osTimerStart(timer, 10000);
    register int i;
    for (i = 0; i < 200; i++) {}
}

int test_timer_suspend() {
    osThreadAttr_t thread1;
    thread1.priority = osPriorityRealtime;
    thread1.attr_bits = osThreadJoinable;
    osKernelInitialize();
    osThreadId_t tid1 = osThreadNew((osThreadFunc_t)osTestTimerSuspend, NULL, &thread1);
    FLEXPRET_ASSERT(tid1 != NULL);
    osKernelStart();

    register int i;
    for (i = 0; i < 200; i++) {}

    osThreadSuspend(tid1);
    osThreadJoin(tid1);

    return PASSED;
}

static void osTestMtfdImm() {
    mti(300);
    register int i;
    for (i = 0; i < 20; i++) {}
    fd();
    // asm volatile ("div x0,x0,x0");
}

static void osTestMtfdImm2() {
    mti(3000);
    register int i;
    for (i = 0; i < 20; i++) {}
    fd();
    // asm volatile ("div x0,x0,x0");
}

// static int osTestMtfdImm(void * arg) {
//     uint32_t time = get_time();
//     mti(1000);
//     register int i;
//     for (i = 0; i < 100; i++) {}
//     set_compare(time + 500);
//     delay_until();
//     fd();
//     return 1;
// }

static void mtfd_trap_handler() {
    // osThreadId_t tid = osThreadGetId();
    uint32_t tid = read_csr(hartid);
    gpo_set_1(0x3);
    
    flexpret_info("Thread ");
    flexpret_info(itoa_hex_removing_ldz(tid));
    flexpret_info(" missed the deadline.\n");
}

static void mtfd_trap_handler2() {
    // osThreadId_t tid = osThreadGetId();
    uint32_t tid = read_csr(hartid);
    gpo_set_2(0x3);
    
    flexpret_info("Thread ");
    flexpret_info(itoa_hex_removing_ldz(tid));
    flexpret_info(" missed the deadline.\n");
}

static void osTestMtfdReg() {
    register int a = 100000;
    // mt(a);
    mti(100000);
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
    osThreadId_t tid2 = osThreadNew((osThreadFunc_t)osTestMtfdImm2, NULL, &thread2);

    // FLEXPRET_ASSERT(tid1 != NULL && tid2 != NULL);
    osThreadSetTrapHandler(tid1, (trap_handler)mtfd_trap_handler, ExceptionOnExpire);
    osThreadSetTrapHandler(tid2, (trap_handler)mtfd_trap_handler2, ExceptionOnExpire);
    osKernelStart();

    osThreadJoin(tid1);
    osThreadJoin(tid2);
    gpo_set_0(0x3);

    return PASSED;
}
