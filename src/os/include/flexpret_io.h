#ifndef FLEXPRET_IO_H
#define FLEXPRET_IO_H

#include "flexpret_encoding.h"
#include "flexpret_const.h"

// GPO ports, if port width < 32, then upper bits ignored
// CSR_GPO_*
// Write all GPO bits
inline void gpo_write_0(uint32_t val);
inline void gpo_write_1(uint32_t val);
inline void gpo_write_2(uint32_t val);
inline void gpo_write_3(uint32_t val);

// For each '1' bit in mask, set corresponding GPO bit to '1'
inline void gpo_set_0(uint32_t mask);
inline void gpo_set_1(uint32_t mask);
inline void gpo_set_2(uint32_t mask);
inline void gpo_set_3(uint32_t mask);

// For each '1' bit in mask, set corresponding GPO bit to '0'
inline void gpo_clear_0(uint32_t mask);
inline void gpo_clear_1(uint32_t mask);
inline void gpo_clear_2(uint32_t mask);
inline void gpo_clear_3(uint32_t mask);

// Read GPO bits
inline uint32_t gpo_read_0();
inline uint32_t gpo_read_1();
inline uint32_t gpo_read_2();
inline uint32_t gpo_read_3();

// GPI ports, if port width < 32, then upper bits are zero
// Read GPI bits
// CSR_GPI_*
inline uint32_t gpi_read_0();
inline uint32_t gpi_read_1();
inline uint32_t gpi_read_2();
inline uint32_t gpi_read_3();

// TODO: move to flexpret_debug?
// Some should be in .c file...

#if defined(DEBUG_EMULATOR) || defined(STATS_EMULATOR)
#define debug_string(s) emulator_outputstr(s);
#else
#define debug_string(s) emulator_outputstr(s);
#endif

// Convert number to string in hex format
char* itoa_hex(uint32_t n);
char* itoa_hex_removing_ldz(uint32_t n);

// Emulator (puts bits on bus)
#define EMULATOR_ADDR 0x40000000

// Write each character in the string to a pre-defined address.
void emulator_outputstr(const char* str);

// TEMP location
inline uint32_t get_cycle();
inline uint32_t get_cycleh();
inline uint32_t get_instret();
inline uint32_t get_instreth();

struct stats{
    uint32_t cycle;
    uint32_t cycleh;
    uint32_t instret;
    uint32_t instreth;
};

inline void stats_get(struct stats* s);

inline void stats_print(struct stats* s, struct stats* e);

#endif
