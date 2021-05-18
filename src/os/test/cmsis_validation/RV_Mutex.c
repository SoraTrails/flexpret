/*-----------------------------------------------------------------------------
 *      Name:         TC_Mutex3.c 
 *      Purpose:      CMSIS RTOS validation tests implementation
 *-----------------------------------------------------------------------------
 *      Copyright(c) KEIL - An ARM Company
 *----------------------------------------------------------------------------*/
// #include <string.h>
#include "cmsis_os2.h"
#include "RV_Framework.h"
#include "flexpret_os.h"

/*-----------------------------------------------------------------------------
 *      Test implementation
 *----------------------------------------------------------------------------*/


/* Definitions of shared variables */
static uint8_t     G_ExecArr[3];               /* Global execution array             */
static osMutexId_t   G_MutexId, G_MutexId2;                  /* Global mutex id                    */
static osThreadId_t  G_Mutex_ThreadId;           /* Global thread id                   */

#define osMutexDef(m) \
  const osMutexAttr_t m = { \
      "myThreadMutex", \
      osMutexRecursive | osMutexRobust, \
      NULL, \
      0U \
  }

/* Definitions for TC_MutexBasic */
osMutexDef (MutexBas);

/* Definitions for TC_MutexNestedAcquire */
static void  Th_MutexWait (void const *arg);
// osThreadDef (Th_MutexWait, osPriorityAboveNormal, 1, 0);

static void RecursiveMutexAcquire (uint32_t depth, uint32_t ctrl);
osMutexDef (Mutex_Nest);

/* Definitions for TC_MutexCheckTimeout */
static void  Th_MutexWakeup (void const *arg);
// osThreadDef (Th_MutexWakeup, osPriorityAboveNormal, 1, 0);
osMutexDef (Mutex_CheckTimeout);

/*-----------------------------------------------------------------------------
 * Recursive mutex acquisition
 *----------------------------------------------------------------------------*/
static void RecursiveMutexAcquire (uint32_t depth, uint32_t ctrl) {
  static uint32_t acq;                  /* Mutex acquisition counter          */
  osStatus_t stat;

  /* Acquire a mutex */
  stat = osMutexAcquire (G_MutexId2, osWaitForever);
  ASSERT_TRUE (stat == osOK);

  if (stat == osOK) {
    if (ctrl == depth) {
      /* - Verify that mutex was acquired at count zero */
      ASSERT_TRUE (acq == 0);
    }
    acq++;

    if (depth) {
      RecursiveMutexAcquire (depth - 1, ctrl);
    }
    acq--;

    /* Release a mutex */
    stat = osMutexRelease (G_MutexId2);
    ASSERT_TRUE (stat == osOK);
  }
}

/*-----------------------------------------------------------------------------
 * Thread waiting for mutex with G_MutexId
 *----------------------------------------------------------------------------*/
static void Th_MutexWait (void const __attribute__((unused)) *arg) {
  RecursiveMutexAcquire (3, 3);
}

/*-----------------------------------------------------------------------------
 *      Wakeup thread
 *----------------------------------------------------------------------------*/
void Th_MutexWakeup (void const __attribute__((unused)) *arg)
{
  /* Acquire mutex */
  ASSERT_TRUE (osMutexAcquire (G_MutexId, 0) == osOK);
  
  osDelay(3000);
  
  /* Release mutex to the main thread */
  ASSERT_TRUE (osMutexRelease (G_MutexId) == osOK);
}

/*-----------------------------------------------------------------------------
 *      Test cases
 *----------------------------------------------------------------------------*/

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\defgroup mutex_func Mutex Functions
\brief Mutex Functions Test Cases
\details
The test cases check the osMutex* functions.

@{
*/

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief Test case: TC_MutexBasic
\details
- Create a mutex object
- Try to release mutex that was not obtained before
- Obtain a mutex object
- Release a mutex object
- Delete a mutex object
*/
void TC_MutexBasic (void) {
  osMutexId_t m_id;

  /* - Create a mutex object */
  m_id = osMutexNew (&MutexBas);
  ASSERT_TRUE (m_id != NULL);
  
  if (m_id != NULL) {
    /* - Try to release mutex that was not obtained before */
    ASSERT_TRUE (osMutexRelease (m_id) == osErrorResource);
    /* - Obtain a mutex object */
    ASSERT_TRUE (osMutexAcquire (m_id, 0) == osOK);
    /* - Release a mutex object */
    ASSERT_TRUE (osMutexRelease (m_id) == osOK);
    /* - Delete a mutex object */
    ASSERT_TRUE (osMutexDelete (m_id)  == osOK);
  }
}


/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief Test case: TC_MutexCheckTimeout
\details
- Set time thresholds
- Create wakeup thread to acquire and release a mutex after 10 ticks
- Wait for a mutex with a defined timeout
- Check if the mutex is acquired between the minimum and maximum thresholds
- Wait for a mutex with an infinite timeout
- Check if the mutex is acquired between the minimum and maximum thresholds
*/
void TC_MutexCheckTimeout (void) {
  osStatus_t stat;
  uint32_t t_min, t_max, t_10;
  
  /* Get main thread ID */
  G_Mutex_ThreadId = osThreadGetId ();
  
  /* Create a semaphore object */
  G_MutexId = osMutexNew (&Mutex_CheckTimeout);
  osMutexSetSpin(G_MutexId, 1);
  ASSERT_TRUE (G_MutexId != NULL);
  
  if (G_MutexId != NULL) {    
    /* Set time thresholds */
    t_min = osKernelGetTickCount();
    osDelay(2700);
    t_min = osKernelGetTickCount() - t_min;

    t_max = osKernelGetTickCount();
    osDelay(5500);
    t_max = osKernelGetTickCount() - t_max;

    /* Create wakeup thread to release a mutex after 10 ticks */
	  osThreadAttr_t attr = { "Th_MutexWakeup", osThreadJoinable, NULL, sizeof(hwthread_state), NULL, 0, osPriorityNormal, 0, 0};
    osThreadId_t Wakeup_Tid = osThreadNew((Th_MutexWakeup), NULL, &attr);
    ASSERT_TRUE (Wakeup_Tid != NULL);
    t_10 = osKernelGetTickCount();
    stat = osMutexAcquire (G_MutexId, 10000);
    t_10 = osKernelGetTickCount() - t_10;
    ASSERT_TRUE (stat == osOK);
    ASSERT_TRUE (t_min < t_10);
    ASSERT_TRUE (t_10 < t_max);
    // osDelay(1000);
    ASSERT_TRUE (osMutexRelease (G_MutexId) == osOK);
    osThreadJoin(Wakeup_Tid);

    /* Create wakeup thread to release a semaphore after 10 ticks */
    Wakeup_Tid = osThreadNew((Th_MutexWakeup), NULL, &attr);
    ASSERT_TRUE (Wakeup_Tid != NULL);
    t_10 = osKernelGetTickCount();
    stat = osMutexAcquire (G_MutexId, osWaitForever);
    t_10 = osKernelGetTickCount() - t_10;
    ASSERT_TRUE (stat == osOK);
    ASSERT_TRUE (t_min < t_10);
    ASSERT_TRUE (t_10 < t_max);
    // osDelay(1000);
    ASSERT_TRUE (osMutexRelease (G_MutexId) == osOK);
    osThreadJoin(Wakeup_Tid);
  }
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief Test case: TC_MutexNestedAcquire
\details
- Create a mutex object
- Obtain a mutex object
- Create a high priority thread that waits for the same mutex
- Recursively acquire and release a mutex object
- Release a mutex
- Verify that every subsequent call released the mutex
- Delete a mutex object
- Mutex object must be released after each acquisition
*/
void TC_MutexNestedAcquire (void) {
  osStatus_t stat;
  
  /* - Create a mutex object */
  G_MutexId2 = osMutexNew (&(Mutex_Nest));
  osMutexSetSpin(G_MutexId2, 1);
  ASSERT_TRUE (G_MutexId2 != NULL);

  if (G_MutexId2  != NULL) {
    /* - Obtain a mutex object */
    stat = osMutexAcquire (G_MutexId2, 0);
    ASSERT_TRUE (stat == osOK);
    
    if (stat == osOK) {
      /* - Create a high priority thread that will wait for the same mutex */
	  osThreadAttr_t attr = { "Th_MutexWait", osThreadJoinable, NULL, sizeof(hwthread_state), NULL, 0, osPriorityNormal, 0, 0};
      G_Mutex_ThreadId = osThreadNew ( (Th_MutexWait), NULL, &attr);
      ASSERT_TRUE (G_Mutex_ThreadId != NULL);
    
      /* - Recursively acquire and release a mutex object */
      RecursiveMutexAcquire (5, 5);

      /* - Release a mutex */
      stat = osMutexRelease (G_MutexId2);
      ASSERT_TRUE (stat == osOK);
      osDelay(11000);

      /* - Verify that every subsequent call released the mutex */
      ASSERT_TRUE (osMutexRelease (G_MutexId2) == osErrorResource);
      osThreadJoin(G_Mutex_ThreadId);
    }
    /* - Delete a mutex object */
    ASSERT_TRUE (osMutexDelete (G_MutexId2) == osOK);
  }
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief Test case: TC_MutexParam
\details
- Test mutex management functions with invalid parameters
*/
void TC_MutexParam (void) {
  ASSERT_TRUE (osMutexNew  (NULL)    == NULL);
  ASSERT_TRUE (osMutexRelease (NULL)    == osErrorParameter);
  ASSERT_TRUE (osMutexAcquire    (NULL, 0) == osErrorParameter);
}

/**
@}
*/ 
// end of group mutex_func

/*-----------------------------------------------------------------------------
 * End of file
 *----------------------------------------------------------------------------*/
