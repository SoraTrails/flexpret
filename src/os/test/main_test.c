#include "cmsis_os2.h"
#include "flexpret_const.h"
#include "flexpret_test.h"
#include "flexpret_kernel.h"
#include "flexpret_utils.h"

int main() {
    int res = 1;
    // res &= test_osKernelGetInfo();
    // res &= test_osKernelGetTickCount();
    // res &= test_osKernelGetTickFreq();
    // res &= test_osThreadNew_osThreadJoin();
    // res &= test_osThread_info();
    // res &= test_osThread_control();
    // res &= test_trap();
    // res &= test_timer();
    // res &= test_timer_suspend();
    res &= test_mtfd();
    // res &= test_mutex(); 
    return !res;
}