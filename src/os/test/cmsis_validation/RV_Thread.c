/*-----------------------------------------------------------------------------
 *      Name:         TC_Thread.c 
 *      Purpose:      CMSIS RTOS Thread Management test
 *-----------------------------------------------------------------------------
 *      Copyright(c) KEIL - An ARM Company
 *----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Modified for CMSIS API v2 for FlexPRET
 *----------------------------------------------------------------------------*/

// #include <string.h>
#include "RV_Framework.h"
#include "cmsis_os2.h"
#include "flexpret_os.h"

/*-----------------------------------------------------------------------------
 *      Test implementation
 *----------------------------------------------------------------------------*/

/* Definitions of shared variables */
static uint32_t Var_Counter;
static void  Th_CountingThread  (void const *arg);

/* Definitions for TC_ThreadCreate */
static volatile uint32_t Var_ThreadExec;

void Th_Thr0 (void const *arg);
void Th_Thr1 (void const *arg);
void Th_Thr2 (void const *arg);
void Th_Thr3 (void const *arg);
void Th_Thr4 (void const *arg);
void Th_Thr5 (void const *arg);
void Th_Thr6 (void const *arg);
void Th_Thr7 (void const *arg);

void Th_Thr0 (void const *arg) { Var_ThreadExec += (arg == NULL) ? (1) : (2); }
void Th_Thr1 (void const *arg) { Var_ThreadExec += (arg == NULL) ? (1) : (2); }
void Th_Thr2 (void const *arg) { Var_ThreadExec += (arg == NULL) ? (1) : (2); }
void Th_Thr3 (void const *arg) { Var_ThreadExec += (arg == NULL) ? (1) : (2); }
void Th_Thr4 (void const *arg) { Var_ThreadExec += (arg == NULL) ? (1) : (2); }
void Th_Thr5 (void const *arg) { Var_ThreadExec += (arg == NULL) ? (1) : (2); }
void Th_Thr6 (void const *arg) { Var_ThreadExec += (arg == NULL) ? (1) : (2); }
void Th_Thr7 (void const *arg) { Var_ThreadExec += (arg == NULL) ? (1) : (2); }

void Th_Run  (void const *arg);

void Th_Run  (void const *arg) { while(1) { osDelay(100);    if(arg!=NULL) return; } }

/* Definitions for TC_ThreadMultiInstance */
void Th_MultiInst (void const *arg);

/* Definitions for TC_ThreadGetId */
#define GETID_THR_CNT 7

void Th_GetId0 (void const *arg);
void Th_GetId1 (void const *arg);
void Th_GetId2 (void const *arg);
void Th_GetId3 (void const *arg);
void Th_GetId4 (void const *arg);
void Th_GetId5 (void const *arg);
void Th_GetId6 (void const *arg);

void Th_GetId0 (void const *arg) { osThreadId_t *p = (osThreadId_t *)arg; *p = osThreadGetId(); }
void Th_GetId1 (void const *arg) { osThreadId_t *p = (osThreadId_t *)arg; *p = osThreadGetId(); }
void Th_GetId2 (void const *arg) { osThreadId_t *p = (osThreadId_t *)arg; *p = osThreadGetId(); }
void Th_GetId3 (void const *arg) { osThreadId_t *p = (osThreadId_t *)arg; *p = osThreadGetId(); }
void Th_GetId4 (void const *arg) { osThreadId_t *p = (osThreadId_t *)arg; *p = osThreadGetId(); }
void Th_GetId5 (void const *arg) { osThreadId_t *p = (osThreadId_t *)arg; *p = osThreadGetId(); }
void Th_GetId6 (void const *arg) { osThreadId_t *p = (osThreadId_t *)arg; *p = osThreadGetId(); }


/* Definitions for TC_ThreadPriorityExec */
static osThreadId_t ExecArr[7];

void Th_PrioExec (void const *arg);

/* Definitions for TC_ThreadChainedCreate */
void Th_Child_0 (void const *arg);
void Th_Child_1 (void const *arg);
void Th_Child_2 (void const *arg);
void Th_Child_3 (void const *arg);

/*-----------------------------------------------------------------------------
 * Counting thread
 *----------------------------------------------------------------------------*/
static void Th_CountingThread  (void const *arg) {
  while(1) {
    Var_Counter++;
    osDelay (5);
    if(arg!=NULL) return;
  }
}

/*-----------------------------------------------------------------------------
 * Auxiliary thread used to test creation of multiple instances
 *----------------------------------------------------------------------------*/
void Th_MultiInst (void const *arg) {
  uint32_t *var = (uint32_t *)arg;
  *var += 1;
  osDelay(10);
}


/*-----------------------------------------------------------------------------
 * Auxiliary thread for testing execution by priority
 *----------------------------------------------------------------------------*/
void Th_PrioExec (void const *arg) {
  uint32_t i;
  for (i = 0; i < 7; i++) {
    if (ExecArr[i] == 0) {
      ExecArr[i] = *(osThreadId_t const *)arg;
      break;
    }
  }
}

/*-----------------------------------------------------------------------------
 * Child threads for TC_ThreadChainedCreate
 *----------------------------------------------------------------------------*/
void Th_Child_0 (void const *arg) {
  uint32_t *arr = (uint32_t *)arg;
  arr[0] = 1;
  ASSERT_TRUE (osThreadNew ( (Th_Child_1), &arr[1], NULL) != NULL);
}
void Th_Child_1 (void const *arg) {
  uint32_t *arr = (uint32_t *)arg;
  arr[0] = 2;
  ASSERT_TRUE (osThreadNew ( (Th_Child_2), &arr[1], NULL) != NULL);
}
void Th_Child_2 (void const *arg) {
  uint32_t *arr = (uint32_t *)arg;
  arr[0] = 3;
  ASSERT_TRUE (osThreadNew ( (Th_Child_3), &arr[1], NULL) != NULL);
}
void Th_Child_3 (void const *arg) {
  uint32_t *arr = (uint32_t *)arg;
  arr[0] = 4;
}

/*-----------------------------------------------------------------------------
 *      Test cases
 *----------------------------------------------------------------------------*/

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\defgroup thread_funcs Thread Functions
\brief Thread Functions Test Cases
\details
The test cases check the * functions.

@{
*/

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief Test case: TC_ThreadCreate
\details
- Create threads with different priorities and pass NULL argument to them
- Create threads with different priorities and pass valid argument to them
*/
void TC_ThreadCreate (void) {
  uint32_t arg = 0xFF;
	const osThreadAttr_t attr[6] = {
		{ "Th_Thr0", osThreadJoinable, NULL, sizeof(hwthread_state), NULL, 0, osPriorityNormal, 0, 0},
		{ "Th_Thr1", osThreadJoinable, NULL, sizeof(hwthread_state), NULL, 0, osPriorityRealtime, 0, 0},
		{ "Th_Thr0", osThreadJoinable, NULL, sizeof(hwthread_state), NULL, 0, osPriorityNormal, 0, 0},
		{ "Th_Thr1", osThreadJoinable, NULL, sizeof(hwthread_state), NULL, 0, osPriorityRealtime, 0, 0},
		{ "Th_Thr7", osThreadJoinable, NULL, sizeof(hwthread_state), NULL, 0, osPriorityError, 0, 0}, // bad param
		{ "Th_Thr7", osThreadJoinable, NULL, sizeof(hwthread_state), NULL, 0, osPriorityError, 0, 0}, // bad param
  };
  Var_ThreadExec = 0;

  /* - Create threads with different priorities and pass NULL argument to them */
  ASSERT_TRUE (osThreadNew ((Th_Thr0), NULL, &attr[0]) != NULL);
  ASSERT_TRUE (osThreadNew ((Th_Thr1), NULL, &attr[1]) != NULL);
  // ASSERT_TRUE (osThreadNew ((Th_Thr2), NULL, NULL) != NULL);
  // ASSERT_TRUE (osThreadNew ((Th_Thr3), NULL, NULL) != NULL);
  // ASSERT_TRUE (osThreadNew ((Th_Thr4), NULL, NULL) != NULL);
  // ASSERT_TRUE (osThreadNew ((Th_Thr5), NULL, NULL) != NULL);
  // ASSERT_TRUE (osThreadNew ((Th_Thr6), NULL, NULL) != NULL);
  
  /* Wait until all threads go into inactive state */
  osDelay (1000);
  
  /* Check if they were all executed */
  ASSERT_TRUE (Var_ThreadExec == 2);
  Var_ThreadExec = 0;

  /* - Create threads with different priorities and pass valid argument to them */
  ASSERT_TRUE (osThreadNew ((Th_Thr0), &arg, &attr[2]) != NULL);
  ASSERT_TRUE (osThreadNew ((Th_Thr1), &arg, &attr[3]) != NULL);
  // ASSERT_TRUE (osThreadNew ((Th_Thr2), &arg, NULL) != NULL);
  // ASSERT_TRUE (osThreadNew ((Th_Thr3), &arg, NULL) != NULL);
  // ASSERT_TRUE (osThreadNew ((Th_Thr4), &arg, NULL) != NULL);
  // ASSERT_TRUE (osThreadNew ((Th_Thr5), &arg, NULL) != NULL);
  // ASSERT_TRUE (osThreadNew ((Th_Thr6), &arg, NULL) != NULL);

  /* Wait until all threads go into inactive state */
  osDelay (1000);

  /* Check if they were all executed */
  ASSERT_TRUE (Var_ThreadExec == 4);

  /* Creating thread with invalid priority should fail */
  ASSERT_TRUE (osThreadNew ((Th_Thr7), NULL, &attr[4]) == NULL);
  ASSERT_TRUE (osThreadNew ((Th_Thr7), &arg, &attr[5]) == NULL);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief Test case: TC_ThreadMultiInstance
\details
- Synchronise on time slice
- Create all defined instances of a thread
- Verify each instance id
- Wait until all instances executed
- Verify execution variable
*/
void TC_ThreadMultiInstance (void) {
  uint32_t   cnt[5];
  uint32_t   i;
  osThreadId_t id[5];

  /* Init counter array */
  for (i = 0; i < 5; i++) {
    cnt[i] = 0;
  }

  /* - Synchronise on time slice */
  osDelay (1);

  /* - Create all defined instances of a thread */
  for (i = 0; i < 5; i++) {
    id[i] = osThreadNew ( (Th_MultiInst), &cnt[i], NULL);
    /* - Verify each instance id */
    ASSERT_TRUE (id[i] != NULL);
  }

  /* - Wait until all instances executed */
  osDelay (1000);
  
  /* - Verify execution variable */
  for (i = 0; i < 5; i++) {
    ASSERT_TRUE (cnt[i] == 1);
  }
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief Test case: TC_ThreadTerminate
\details
- Initialize global counter variable to zero
- Create and run a test thread which increments a global counter
- Terminate test thread and remember counter value
- Wait and verify that thread is terminated by checking global counter
- Restart a thread and repeat the procedure
- Counter must not increment after test thread is terminated
*/
void TC_ThreadTerminate (void) {
  uint32_t i, cnt;
  osThreadId_t id;
  
  for (i = 0; i < 2; i++) {
    /* - Initialize global counter variable to zero */
    Var_Counter = 0;

    /* - Create and run a test thread which increments a global counter */
    id = osThreadNew ( (Th_CountingThread), NULL, NULL);
    ASSERT_TRUE (id != NULL);

    if (id != NULL) {
      /* Allow counting thread to run */
      ASSERT_TRUE (osDelay(5) == osOK);
      /* - Terminate test thread and remember counter value */
      ASSERT_TRUE (osThreadTerminate (id) == osOK);
      ASSERT_TRUE (Var_Counter != 0);
      cnt = Var_Counter;

      ASSERT_TRUE (osDelay(5) == osOK);
      /* - Wait and verify that thread is terminated by checking global counter */
      ASSERT_TRUE (cnt == Var_Counter);
    }
  }
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief Test case: TC_ThreadRestart
\details
- Create a counting thread and verify that it runs
- Terminate counting thread
- Recreate a counting thread and verify that is runs again
*/
void TC_ThreadRestart (void) {
  osThreadId_t id;
  uint32_t   cnt, iter;
  
  /* Initalize global counter variable to zero */
  Var_Counter = 0;
  
  cnt  = 0;
  iter = 0;
  while (iter++ < 2) {
    /* - Create a counting thread and verify that it runs */
    id = osThreadNew ( (Th_CountingThread), NULL, NULL);
    ASSERT_TRUE (id != NULL);
  
    if (id != NULL) {
      /* Allow counting thread to run */
      ASSERT_TRUE (osDelay(5) == osOK);
      ASSERT_TRUE (osThreadTerminate (id) == osOK);

      if (iter == 1) {
        ASSERT_TRUE (Var_Counter != 0);
        cnt = Var_Counter;
      }
      else {
        ASSERT_TRUE (Var_Counter > cnt);
      }
    }
  }
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief Test case: TC_ThreadGetId
\details
- Create multiple threads
- Each thread must call osThreadGetId and return its thread ID to the main thread
- Check if each thread returned correct ID
- osThreadGetId must return same ID as obtained by osThreadNew
*/
void TC_ThreadGetId (void) {
  osThreadId_t id[2][7];
  osThreadId_t id_main;
  uint32_t i;

  /* - Create multiple threads */
  id[0][0] = osThreadNew ( (Th_GetId0), &id[1][0], NULL);
  id[0][1] = osThreadNew ( (Th_GetId1), &id[1][1], NULL);
  id[0][2] = osThreadNew ( (Th_GetId2), &id[1][2], NULL);
  id[0][3] = osThreadNew ( (Th_GetId3), &id[1][3], NULL);
  id[0][4] = osThreadNew ( (Th_GetId4), &id[1][4], NULL);
  id[0][5] = osThreadNew ( (Th_GetId5), &id[1][5], NULL);
  id[0][6] = osThreadNew ( (Th_GetId6), &id[1][6], NULL);

  /* Check if all threads were created */
  for (i = 0; i < 7; i++) {
    ASSERT_TRUE (id[0][i] != NULL);
  }

  /* Lower priority of the main thread */
  id_main = osThreadGetId();
  ASSERT_TRUE (id_main != NULL);
  
  if (id_main != NULL) {
    ASSERT_TRUE (osThreadSetPriority (id_main, osPriorityLow) == osOK);
    /* - Check if each thread returned correct ID */
    for (i = 0; i < 7; i++) {
      ASSERT_TRUE (id[0][i] == id[1][i]);
    }
    /* Restore priority of the main thread back to normal */
    ASSERT_TRUE (osThreadSetPriority (id_main, osPriorityNormal) == osOK);
  }
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief Test case: TC_ThreadPriority
\details
- Change priority of the main thread in steps from Idle to Realtime
- At each step check if priority was changed
- Do the same for main child thread
*/
void TC_ThreadPriority (void) {
  osThreadId_t t_idc, t_idr;
  
  /* Get main thread ID */
  t_idc = osThreadGetId();
  ASSERT_TRUE (t_idc != NULL);
  
  if (t_idc) {
    /* - Change priority of the main thread in steps from Idle to Realtime  */
    /* - At each step check if priority was changed                         */
    ASSERT_TRUE (osThreadSetPriority (t_idc, osPriorityIdle        ) == osOK);
    ASSERT_TRUE (osThreadGetPriority (t_idc) == osPriorityIdle);
    
    ASSERT_TRUE (osThreadSetPriority (t_idc, osPriorityLow         ) == osOK);
    ASSERT_TRUE (osThreadGetPriority (t_idc) == osPriorityLow);
    
    ASSERT_TRUE (osThreadSetPriority (t_idc, osPriorityBelowNormal ) == osOK);
    ASSERT_TRUE (osThreadGetPriority (t_idc) == osPriorityBelowNormal);
    
    ASSERT_TRUE (osThreadSetPriority (t_idc, osPriorityNormal      ) == osOK);
    ASSERT_TRUE (osThreadGetPriority (t_idc) == osPriorityNormal);
    
    ASSERT_TRUE (osThreadSetPriority (t_idc, osPriorityAboveNormal ) == osOK);
    ASSERT_TRUE (osThreadGetPriority (t_idc) == osPriorityAboveNormal);
    
    ASSERT_TRUE (osThreadSetPriority (t_idc, osPriorityHigh        ) == osOK);
    ASSERT_TRUE (osThreadGetPriority (t_idc) == osPriorityHigh);
    
    ASSERT_TRUE (osThreadSetPriority (t_idc, osPriorityRealtime    ) == osOK);
    ASSERT_TRUE (osThreadGetPriority (t_idc) == osPriorityRealtime);
    
    /* Setting invalid priority value should fail */
    ASSERT_TRUE (osThreadSetPriority (t_idc, osPriorityError) != osOK);
  }


  /* Create a child thread */
  t_idr = osThreadNew ( (Th_Run), NULL, NULL);
  ASSERT_TRUE (t_idr != NULL);
  
  if (t_idr) {
    /* - Change priority of the child thread in steps from Idle to Realtime  */
    /* - At each step check if priority was changed                          */
    ASSERT_TRUE (osThreadGetPriority (t_idr) == osPriorityIdle);
    
    ASSERT_TRUE (osThreadSetPriority (t_idr, osPriorityLow         ) == osOK);
    ASSERT_TRUE (osThreadGetPriority (t_idr) == osPriorityLow);
    
    ASSERT_TRUE (osThreadSetPriority (t_idr, osPriorityBelowNormal ) == osOK);
    ASSERT_TRUE (osThreadGetPriority (t_idr) == osPriorityBelowNormal);
    
    ASSERT_TRUE (osThreadSetPriority (t_idr, osPriorityNormal      ) == osOK);
    ASSERT_TRUE (osThreadGetPriority (t_idr) == osPriorityNormal);
    
    ASSERT_TRUE (osThreadSetPriority (t_idr, osPriorityAboveNormal ) == osOK);
    ASSERT_TRUE (osThreadGetPriority (t_idr) == osPriorityAboveNormal);
    
    ASSERT_TRUE (osThreadSetPriority (t_idr, osPriorityHigh        ) == osOK);
    ASSERT_TRUE (osThreadGetPriority (t_idr) == osPriorityHigh);
    
    ASSERT_TRUE (osThreadSetPriority (t_idr, osPriorityRealtime    ) == osOK);
    ASSERT_TRUE (osThreadGetPriority (t_idr) == osPriorityRealtime);

    /* Setting invalid priority value should fail */
    ASSERT_TRUE (osThreadSetPriority (t_idr, osPriorityError) != osOK);
    
    /* Terminate the thread */
    ASSERT_TRUE (osThreadTerminate (t_idr) == osOK);
  }

  /* Restore priority of the main thread back to normal */
  ASSERT_TRUE (osThreadSetPriority (t_idc, osPriorityNormal) == osOK);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief Test case: TC_ThreadPriorityExec
\details
- Raise main thread priority to osPriorityRealtime
- Create multiple instances of the test thread
- Set instance priorities from low to realtime
- Lower main thread priority to allow execution of test threads
- Check if threads were executed according to their priorities
*/
void TC_ThreadPriorityExec (void) {
  uint32_t i,j;
  osThreadId_t main_id, inst[7];
  
  main_id = osThreadGetId();
  ASSERT_TRUE (main_id != NULL);

  if (main_id != NULL) {
    /* - Raise main thread priority to osPriorityRealtime */
    ASSERT_TRUE (osThreadSetPriority (main_id, osPriorityRealtime) == osOK);

    /* - Create multiple instances of the test thread */
    inst[0] = osThreadNew ( (Th_PrioExec), (void *)&inst[0], NULL);
    inst[1] = osThreadNew ( (Th_PrioExec), (void *)&inst[1], NULL);
    inst[2] = osThreadNew ( (Th_PrioExec), (void *)&inst[2], NULL);
    inst[3] = osThreadNew ( (Th_PrioExec), (void *)&inst[3], NULL);
    inst[4] = osThreadNew ( (Th_PrioExec), (void *)&inst[4], NULL);
    inst[5] = osThreadNew ( (Th_PrioExec), (void *)&inst[5], NULL);
    inst[6] = osThreadNew ( (Th_PrioExec), (void *)&inst[6], NULL);

    for (i = 0; i < 7; i++) {
      ASSERT_TRUE (inst[i] != NULL);
      if (inst[i] == NULL) {
        /* Abort test if thread instance not created */
        return;
      }
      /* Check if the instances are different */
      for (j = 0; j < 7; j++) {
        if (i != j) {
            ASSERT_TRUE (inst[i] != inst[j]);
        }
      }
    }  
    
    /* Clear execution array */
    for (i = 0; i < 7; i++) {
      ExecArr[i] = 0;
    }

    /* - Set instance priorities from low to realtime */
    ASSERT_TRUE (osThreadSetPriority (inst[0], osPriorityIdle)        == osOK);
    ASSERT_TRUE (osThreadSetPriority (inst[1], osPriorityLow)         == osOK);
    ASSERT_TRUE (osThreadSetPriority (inst[2], osPriorityBelowNormal) == osOK);
    ASSERT_TRUE (osThreadSetPriority (inst[3], osPriorityNormal)      == osOK);
    ASSERT_TRUE (osThreadSetPriority (inst[4], osPriorityAboveNormal) == osOK);
    ASSERT_TRUE (osThreadSetPriority (inst[5], osPriorityHigh)        == osOK);
    ASSERT_TRUE (osThreadSetPriority (inst[6], osPriorityRealtime)    == osOK);
    
    /* - Lower main thread priority to allow execution of test threads */
    ASSERT_TRUE (osThreadSetPriority (main_id, osPriorityIdle)        == osOK);
    ASSERT_TRUE (osThreadYield () == osOK);
    
    /* - Check if threads were executed according to their priorities */
    for (i = 0; i < 7; i++) {
      ASSERT_TRUE (ExecArr[i] == inst[7 - 1 - i]);
    }
  }
  /* - Restore main thread priority to osPriorityNormal */
  ASSERT_TRUE (osThreadSetPriority (main_id, osPriorityNormal) == osOK);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief Test case: TC_ThreadChainedCreate
\details
- Create a first thread in a chain
- Wait until child threads are created
- Verify that all child threads were created
*/
void TC_ThreadChainedCreate (void) {
  uint32_t i, arr[4];
  osThreadId_t id;
  
  /* Clear array */
  for (i = 0; i < 4; i++) {
    arr[i] = 0;
  }
  
  /* - Create a first thread in a chain */
  id = osThreadNew (  (Th_Child_0), arr, NULL);
  ASSERT_TRUE (id != NULL);
  
  /* - Wait until child threads are created */
  osDelay(50);
  
  /* - Verify that all child threads were created */
  for (i = 0; i < 4; i++) {
    ASSERT_TRUE (arr[i] == (i + 1));
  }
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief Test case: TC_ThreadParam
\details
- Test thread management functions with invalid parameters
*/
void TC_ThreadParam (void) {
  /* - Test invalid thread definition on osThreadNew */
  ASSERT_TRUE (osThreadNew (NULL, NULL, NULL) == NULL);
  
  /* - Test invalid thread id on osThreadTerminate */
  ASSERT_TRUE (osThreadTerminate (NULL) == osErrorParameter);
  
  /* - Test invalid thread id on osThreadSetPriority */
  ASSERT_TRUE (osThreadSetPriority (NULL, osPriorityNormal) == osErrorParameter);
  
  /* - Test invalid thread id on osThreadGetPriority */
  ASSERT_TRUE (osThreadGetPriority (NULL) == osPriorityError);
}

/**
@}
*/ 
// end of group thread_funcs

/*-----------------------------------------------------------------------------
 * End of file
 *----------------------------------------------------------------------------*/
