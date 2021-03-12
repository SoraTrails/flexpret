#include "flexpret_io.h"

void gpo_write_0(uint32_t val) { write_csr(uarch4, val); }
void gpo_write_1(uint32_t val) { write_csr(uarch5, val); }
void gpo_write_2(uint32_t val) { write_csr(uarch6, val); }
void gpo_write_3(uint32_t val) { write_csr(uarch7, val); }

void gpo_set_0(uint32_t mask) { set_csr(uarch4, mask); }
void gpo_set_1(uint32_t mask) { set_csr(uarch5, mask); }
void gpo_set_2(uint32_t mask) { set_csr(uarch6, mask); }
void gpo_set_3(uint32_t mask) { set_csr(uarch7, mask); }

void gpo_clear_0(uint32_t mask) { clear_csr(uarch4, mask); }
void gpo_clear_1(uint32_t mask) { clear_csr(uarch5, mask); }
void gpo_clear_2(uint32_t mask) { clear_csr(uarch6, mask); }
void gpo_clear_3(uint32_t mask) { clear_csr(uarch7, mask); }

uint32_t gpo_read_0() { return read_csr(uarch4); }
uint32_t gpo_read_1() { return read_csr(uarch5); }
uint32_t gpo_read_2() { return read_csr(uarch6); }
uint32_t gpo_read_3() { return read_csr(uarch7); }

uint32_t gpi_read_0() { return read_csr(uarch0); }
uint32_t gpi_read_1() { return read_csr(uarch1); }
uint32_t gpi_read_2() { return read_csr(uarch2); }
uint32_t gpi_read_3() { return read_csr(uarch3); }

uint32_t get_cycle() { return read_csr(cycle); }
uint32_t get_cycleh() { return read_csr(cycleh); }
uint32_t get_instret() { return read_csr(instret); }
uint32_t get_instreth() { return read_csr(instreth); }

void stats_get(struct stats* s) {
    do {
        s->cycleh = get_cycleh();
        s->instreth = get_instreth();
        s->cycle = get_cycle();
        s->instret = get_instret();
    } while((get_cycleh() != s->cycleh) || (get_instreth() != s->instreth));
}

void stats_print(struct stats* s, struct stats* e) {
    debug_string(itoa_hex(e->cycleh - s->cycleh));
    debug_string(itoa_hex(e->cycle - s->cycle));
    debug_string("\n");
    debug_string(itoa_hex(e->instreth - s->instreth));
    debug_string(itoa_hex(e->instret - s->instret));
    debug_string("\n");
}

void emulator_outputstr(const char* str) {
    volatile char* addr = (char*) EMULATOR_ADDR;
    while(*str != 0) {
        *addr = *str;
        str++;
    }
}

char qbuf[9];

char* itoa_hex(uint32_t n)
{
    register int i;
    for (i = 7; i >= 0; i--) {
        qbuf[i] = (n & 15) + 48;
        if(qbuf[i] >= 58) {
            qbuf[i] += 7;
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
    qbuf[8] = '\0';

    return(qbuf);
}

