#ifndef FLEXPRET_SCHEDULER_H
#define FLEXPRET_SCHEDULER_H
#include "flexpret_const.h"
#include "flexpret_utils.h"
#include "cmsis_os2.h"

#define THREAD_FREQ(res)     ((res >> 8) & 0xF)
#define SOFT_SLOT_COUNT(res) ((res >> 4) & 0xF)
#define ALL_SLOT_COUNT(res)  ((res >> 0) & 0xF)

// static uint32_t slots;
// static uint16_t tmodes;

/// Get Schedule Info, including schedule Frequency of a thread, soft slot count and all slot count
/// \param[in]     thread_id     thread ID obtained by \ref osThreadNew or \ref osThreadGetId.
/// \return 0-3bit: all slot count; 4-7bit: soft slot count; 8-11bit: slot holding by thread.
int32_t osSchedulerGetFreq(osThreadId_t tid);

/// Set schedule slot of a thread.
/// \param[in]     thread_id     thread ID obtained by \ref osThreadNew or \ref osThreadGetId.
/// \param[in]     count         schedule slot count to be Set.
/// \return status code that indicates the execution status of the function.
osStatus_t osSchedulerSetSlotNum(osThreadId_t tid, int count);

/// Set schedule slot for SRTT.
/// \param[in]     count         soft slot count to be set.
/// \return status code that indicates the execution status of the function.
osStatus_t osSchedulerSetSoftSlotNum(int count);

/// Set thread mode of a thread.
/// \param[in]     thread_id     thread ID obtained by \ref osThreadNew or \ref osThreadGetId.
/// \param[in]     tmode         thread mode to be Set.
/// \return status code that indicates the execution status of the function.
osStatus_t osSchedulerSetTmodes(osThreadId_t tid, int tmode);

/// Get thread mode of a thread.
/// \param[in]     thread_id     thread ID obtained by \ref osThreadNew or \ref osThreadGetId.
/// \return thread mode.
int osSchedulerGetTmodes(osThreadId_t tid);

/// Get SRTT Num.
/// \return SRTT Num.
int32_t osSchedulerGetSRTTNum();


#endif

