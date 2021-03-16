#ifndef FLEXPRET_THREADS_H
#define FLEXPRET_THREADS_H

#include "flexpret_encoding.h"
#include "flexpret_const.h"

#ifndef FLEXPRET_HW_THREADS_NUMS
#define FLEXPRET_HW_THREADS_NUMS 4
#endif

#ifndef FLEXPRET_MAX_HW_THREADS_NUMS
#define FLEXPRET_MAX_HW_THREADS_NUMS 8
#endif

#ifndef FLEXPRET_SW_THREADS_NUMS
#define FLEXPRET_SW_THREADS_NUMS 16
#endif

// CSR_SLOTS
#define set_slots(s7, s6, s5, s4, s3, s2, s1, s0) (\
        {swap_csr(badvaddr, (\
        ((s7 & 0xF) << 28) | \
        ((s6 & 0xF) << 24) | \
        ((s5 & 0xF) << 20) | \
        ((s4 & 0xF) << 16) | \
        ((s3 & 0xF) << 12) | \
        ((s2 & 0xF) <<  8) | \
        ((s1 & 0xF) <<  4) | \
        ((s0 & 0xF) <<  0)   \
        ));})

#define get_slots(p_s0, p_s1, p_s2, p_s3, p_s4, p_s5, p_s6, p_s7) ( \
    {   \
        long __tmp; \
        asm volatile ("csrr %0, badvaddr" : "=r"(__tmp)); \
        *(p_s0) = (__tmp >>  0) & 0xF; \
        *(p_s1) = (__tmp >>  4) & 0xF; \
        *(p_s2) = (__tmp >>  8) & 0xF; \
        *(p_s3) = (__tmp >> 12) & 0xF; \
        *(p_s4) = (__tmp >> 16) & 0xF; \
        *(p_s5) = (__tmp >> 20) & 0xF; \
        *(p_s6) = (__tmp >> 24) & 0xF; \
        *(p_s7) = (__tmp >> 28) & 0xF; \
    })


#define SLOT_T0 0
#define SLOT_T1 1
#define SLOT_T2 2
#define SLOT_T3 3
#define SLOT_T4 4
#define SLOT_T5 5
#define SLOT_T6 6
#define SLOT_T7 7
#define SLOT_S 14
#define SLOT_D 15

// CSR_TMODES
#define set_tmodes_4(t3, t2, t1, t0) (\
        {write_csr(ptbr, (\
        ((t3 & 0x3) << 6) | \
        ((t2 & 0x3) << 4) | \
        ((t1 & 0x3) << 2) | \
        ((t0 & 0x3) << 0)   \
        ));})
#define get_tmodes_4(p_t0, p_t1, p_t2, p_t3) ( \
    {   \
        long __tmp; \
        asm volatile ("csrr %0, ptbr" : "=r"(__tmp)); \
        *(p_t0) = __tmp & 0x3; \
        *(p_t1) = (__tmp >> 2) & 0x3; \
        *(p_t2) = (__tmp >> 4) & 0x3; \
        *(p_t3) = (__tmp >> 6) & 0x3; \
    })
#define TMODE_HA 0
#define TMODE_HZ 1
#define TMODE_SA 2
#define TMODE_SZ 3

// Memory Protection
#define MEMP_T0 0
#define MEMP_T1 1
#define MEMP_T2 2
#define MEMP_T3 3
#define MEMP_T4 4
#define MEMP_T5 5
#define MEMP_T6 6
#define MEMP_T7 7
#define MEMP_SH 8
#define MEMP_RO 9

typedef struct hwthread_state {
    void (*func)(void* arg);
    void * arg;
    void * stack_address;
} hwthread_state;

typedef enum {
    hard  =  0,
    soft  =  1,
} thread_type;

/// Attributes structure for thread, in cmsis_os2.h.
// typedef struct {
//   const char                   *name;   ///< name of the thread
//   uint32_t                 attr_bits;   ///< attribute bits
//   void                      *cb_mem;    ///< memory for control block
//   uint32_t                   cb_size;   ///< size of provided memory for control block
//   void                   *stack_mem;    ///< memory for stack
//   uint32_t                stack_size;   ///< size of stack
//   osPriority_t              priority;   ///< initial thread priority (default: osPriorityNormal)
//   TZ_ModuleId_t            tz_module;   ///< TrustZone module identifier
//   uint32_t                  reserved;   ///< reserved (must be 0)
// } osThreadAttr_t;

// When priority is osPriorityRealtime, HRTT hw thread is created.
// When priority is osPriorityNormal, SRTT hw thread is created.
// More priority(sw thread) is not supported currently, default priority is osPriorityNormal.

#endif
