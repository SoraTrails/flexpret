#include "cmsis_os2.h"
#include "flexpret_const.h"
#include "flexpret_test.h"
#include "flexpret_kernel.h"
#include "flexpret_utils.h"

int main() {
    int res = 1;
    res &= test_osKernelGetInfo();

    return !res;
}