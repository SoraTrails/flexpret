#ifndef FLEXPRET_TEST_H
#define FLEXPRET_TEST_H
#include "flexpret_utils.h"
#include "flexpret_encoding.h"

#define FLEXPRET_TEST_FAILED2(f,l) flexpret_error("Test case failed at file " f "(line:" #l ")\n")
#define FLEXPRET_TEST_FAILED1(f,l) FLEXPRET_TEST_FAILED2(f,l)
#define FLEXPRET_TEST_FAILED() FLEXPRET_TEST_FAILED1(__FILE__, __LINE__)

#define PASSED 1
#define FAILED 0

#define FLEXPRET_ASSERT(e) \
    { \
        if (!(e)) { \
            FLEXPRET_TEST_FAILED(); \
            return FAILED; \
        } \
    }

#define FLEXPRET_THREAD_ASSERT(e) \
    { \
        if (!(e)) { \
            FLEXPRET_TEST_FAILED(); \
            return ; \
        } \
    }

#define FLEXPRET_DEBUG_SLOT_TMODE() \
    { \
        uint32_t _tmp_slot = read_csr(badvaddr); \
        uint32_t _tmp_tmode = read_csr(ptbr); \
        flexpret_info("[debug] slot: "); \
        flexpret_info(itoa_hex(_tmp_slot)); \
        flexpret_info(", tmode: "); \
        flexpret_info(itoa_hex(_tmp_tmode)); \
        flexpret_info("\n"); \
    }

// Test Kernel api
int test_osKernelGetInfo();
int test_osKernelGetTickCount();
int test_osKernelGetTickFreq();

// Test Thread api
int test_osThreadNew_osThreadJoin();
int test_osThread_info();
int test_osThread_control();

// Test trap
int test_trap();

// Test timer
int test_timer();
int test_timer_suspend();
int test_mtfd();

// Test mutex
int test_mutex();

#endif