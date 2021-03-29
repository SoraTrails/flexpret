#include "flexpret_io.h"
#include "flexpret_threads.h"

static char qbuf[FLEXPRET_HW_THREADS_NUMS][9];

void emulator_outputstr(const char* str) {
    if (!str) {
        return;
    }
    volatile char* addr = (char*) (EMULATOR_ADDR);
    while(*str != 0) {
        *addr = *str;
        str++;
    }
}

// TODO : itoa_hex & itoa_hex_removing_ldz is not thread safe
char* itoa_hex(uint32_t n)
{
    register int i;
    uint32_t tid = read_csr(hartid);
    for (i = 7; i >= 0; i--) {
        qbuf[tid][i] = (n & 15) + 48;
        if(qbuf[tid][i] >= 58) {
            qbuf[tid][i] += 7;
        }
        n = n >> 4;
    }
    // qbuf[0] = ((n & 0xf0000000) >> 28) + '0';
    // qbuf[1] = ((n & 0x0f000000) >> 24) + '0';
    // qbuf[2] = ((n & 0x00f00000) >> 20) + '0';
    // qbuf[3] = ((n & 0x000f0000) >> 16) + '0';
    // qbuf[4] = ((n & 0x0000f000) >> 12) + '0';
    // qbuf[5] = ((n & 0x00000f00) >>  8) + '0';
    // qbuf[6] = ((n & 0x000000f0) >>  4) + '0';
    // qbuf[7] = ((n & 0x0000000f) >>  0) + '0';
    // for (i = 0; i < 8; i++) {
    //     if (qbuf[i] > '9') {
    //         qbuf[i] += 'a' - '9';
    //     }
    // }
    qbuf[tid][8] = '\0';

    return(qbuf[tid]);
}

// remove leading zero
char* itoa_hex_removing_ldz(uint32_t n)
{
    register int i;
    uint32_t tid = read_csr(hartid);
    for (i = 7; i >= 0; i--) {
        qbuf[tid][i] = (n & 15) + 48;
        if(qbuf[tid][i] >= 58) {
            qbuf[tid][i] += 7;
        }
        n = n >> 4;
    }
    qbuf[tid][8] = '\0';

    for (i = 0; i < 7 && qbuf[tid][i] == '0'; i++) 
        ;
    return (qbuf[tid] + i);
}

