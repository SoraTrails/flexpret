#ifndef FLEXPRET_TEST_H
#define FLEXPRET_TEST_H
#include "flexpret_utils.h"

#define FLEXPRET_TEST_FAILED2(f,l) flexpret_error("Test case failed at file " f "(line:" #l ")\n")
#define FLEXPRET_TEST_FAILED1(f,l) FLEXPRET_TEST_FAILED2(f,l)
#define FLEXPRET_TEST_FAILED() FLEXPRET_TEST_FAILED1(__FILE__, __LINE__)

// Test Kernel api
int test_osKernelGetInfo();


#endif