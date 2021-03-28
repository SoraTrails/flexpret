#include "cmsis_os2.h"
#include "flexpret_const.h"
#include "flexpret_test.h"
#include "flexpret_kernel.h"
#include "flexpret_utils.h"
#include "flexpret_io.h"

// int test_osKernelGetInfo() {
//     osVersion_t version;
//     char id_buf[64];
//     size_t id_size = 64;

//     id_buf[0] = '\0';
//     id_buf[63] = '\0';
//     osStatus_t status = osKernelGetInfo(&version, id_buf, id_size);
//     if (status != osOK) {
//         FLEXPRET_TEST_FAILED();
//         return FAILED;
//     }
//     if (strlen(id_buf) == 0) {
//         FLEXPRET_TEST_FAILED();
//         return FAILED;
//     }
//     flexpret_info("Flexpret version info: ");
//     flexpret_info(id_buf);
//     return PASSED;
// }

// int test_osKernelGetTickCount() {
//     uint32_t time1 = osKernelGetTickCount();
//     flexpret_info("Current time is ");
//     flexpret_info(itoa_hex_removing_ldz(time1));
//     flexpret_info("\n");

//     register int i;
//     for (i = 50; i >= 0; i--)
//         ;
//     uint32_t time2 = osKernelGetTickCount();
//     flexpret_info("Current time is ");
//     flexpret_info(itoa_hex_removing_ldz(time2));
//     flexpret_info("\n");
//     return PASSED;
// }

// int test_osKernelGetTickFreq() {
//     uint32_t freq = osKernelGetTickFreq();
//     flexpret_info("Current freq is ");
//     flexpret_info(itoa_hex_removing_ldz(freq));
//     flexpret_info("\n");
//     return PASSED;
// }
