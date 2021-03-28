#ifndef FLEXPRET_TIMING_H
#define FLEXPRET_TIMING_H
#include "cmsis_os2.h"
#include "flexpret_encoding.h"
#include "flexpret_const.h"

#ifndef FLEXPRET_FREQ
#define FLEXPRET_FREQ 100000000 // 100Mhz
#endif

#ifndef FLEXPRET_WAIT_PERIOD
#define FLEXPRET_WAIT_PERIOD 10000 // 10000 cycle
#endif

// Timing instructions
// Read current time
// CSR_CLOCK
static inline uint32_t get_time() { return read_csr(fflags); }
// Set value to compare against current time
// Also, set to anything to disable interrupt or exception on expire
// CSR_COMPARE
static inline void set_compare(uint32_t val) { write_csr(compare, val); }
// TODO: support read?
// Delay until compare expires (even when returning from interrupt/exception)
// TI_DU
static inline void delay_until() { asm volatile ("custom0 zero, zero, zero, 0"); };
// Wait until compare expires (unless returning from interrupt/exception)
// TI_WU
static inline void wait_until() { asm volatile ("custom1 zero, zero, zero, 0"); };
// Interrupt execution when compare expires (cause = CAUSE_IE)
// TI_EE
static inline void interrupt_expire() { asm volatile ("custom2 zero, zero, zero, 1"); };
// Throw exception when compare expires (cause = CAUSE_EE)
// TI_IE
static inline void exception_expire() { asm volatile ("custom2 zero, zero, zero, 0"); };

// Helper functions for timing instructions

// Increment time by period and delay until that time
static inline void delay_until_periodic(unsigned int* time, unsigned int period) {
    *time += period;
    set_compare(*time);
    delay_until();
}

// TODO: move?
static inline void enable_interrupts() { asm volatile ("csrrsi x0, status, 16"); }
static inline void disable_interrupts() { asm volatile ("csrrci x0, status, 16"); }
// TODO: handler (state)


// Emulator support
static inline void  counters_start() { asm volatile ("csrrsi x0, uarch14,1"); }
static inline void  counters_end() { asm volatile ("csrrsi x0, uarch15,1"); }

typedef struct hwtimer_state {
    uint32_t ticks;
    osTimerFunc_t func;
    int type;
    void* arg;
} hwtimer_state;

/// Get timer id of a thread, cause flexpret only supports one thread has one timer.
/// \param[in]     thread_id     thread ID obtained by \ref osThreadNew or \ref osThreadGetId.
/// \return timer ID for reference by other functions or NULL in case of error.
osTimerId_t osThreadGetTimer (osThreadId_t thread_id);

#endif
