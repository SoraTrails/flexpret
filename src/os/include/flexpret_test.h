#ifndef FLEXPRET_TEST_H
#define FLEXPRET_TEST_H
#include "flexpret_utils.h"

#define FLEXPRET_TEST_FAILED2(f,l) flexpret_error("Test case failed at file " f "(line:" #l ")\n")
#define FLEXPRET_TEST_FAILED1(f,l) FLEXPRET_TEST_FAILED2(f,l)
#define FLEXPRET_TEST_FAILED() FLEXPRET_TEST_FAILED1(__FILE__, __LINE__)

#define PASSED 1
#define FAILED 0

// Test Kernel api
int test_osKernelGetInfo();
int test_osKernelGetTickCount();
int test_osKernelGetTickFreq();

// Test Thread api
int test_osThreadNew_osThreadJoin();
int test_osThread_others();

#endif