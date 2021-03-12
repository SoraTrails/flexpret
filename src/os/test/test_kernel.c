#include "cmsis_os2.h"
#include "flexpret_const.h"
#include "flexpret_test.h"
#include "flexpret_kernel.h"
#include "flexpret_utils.h"

int test_osKernelGetInfo() {
    osVersion_t version;
    char id_buf[64];
    size_t id_size = 64;

    id_buf[0] = '\0';
    osStatus_t status = osKernelGetInfo(&version, id_buf, id_size);
    if (status != osOK) {
        FLEXPRET_TEST_FAILED();
        return 0;
    }
    if (strlen(id_buf) == 0) {
        FLEXPRET_TEST_FAILED();
        return 0;
    }
    flexpret_info(id_buf);
    return 1;
}
