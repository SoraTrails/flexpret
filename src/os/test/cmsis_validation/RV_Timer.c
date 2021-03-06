/*-----------------------------------------------------------------------------
 *      Name:         RV_Timer.c 
 *      Purpose:      CMSIS RTOS validation tests implementation
 *-----------------------------------------------------------------------------
 *      Copyright(c) KEIL - An ARM Company
 *----------------------------------------------------------------------------*/
#include "RV_Framework.h"
#include "cmsis_rv.h"

/*-----------------------------------------------------------------------------
 *      Test implementation
 *----------------------------------------------------------------------------*/

/* Timer callback prototypes */
void TimCb_Oneshot  (void const *arg);
void TimCb_Periodic (void const *arg);

static volatile uint32_t Tim_Var, Tim_Var_Os, Tim_Var_Per;

/*-----------------------------------------------------------------------------
 *      One shoot timer
 *----------------------------------------------------------------------------*/
void TimCb_Oneshot  (void const *arg) {
  uint32_t var = *(uint32_t *)arg;
  
  ASSERT_TRUE (var != 0);
  Tim_Var_Os += 1;
}


/*-----------------------------------------------------------------------------
 *      Periodic timer
 *----------------------------------------------------------------------------*/
void TimCb_Periodic (void const *arg) {
  uint32_t var = *(uint32_t *)arg;
  
  ASSERT_TRUE (var != 0);
  Tim_Var_Per += 1;
}

/*-----------------------------------------------------------------------------
 *      Test cases
 *----------------------------------------------------------------------------*/

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\defgroup timer_func Timer Functions
\brief Timer Functions Test Cases
\details
The test cases check the osTimer* functions.

@{
*/

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief Test case: TC_TimerOneShot
\details
- Create a one shoot timer
- Ensure timer is not running
- Start timer and check that callback was called only once
- Ensure correct status reporting
- Delete the timer
*/
void TC_TimerOneShot (void) {
  uint32_t  i, var = 1;
  osTimerId_t id;

  /* - Create a one shoot timer */
  id = osTimerNew (TimCb_Oneshot, osTimerOnce, &var, NULL);
  ASSERT_TRUE (id != NULL);

  if (id) {
    /* - Ensure timer is not running */
    ASSERT_TRUE (osTimerStop (id) == osErrorResource);
    
    /* - Start timer and check that callback was called only once */
    ASSERT_TRUE (osTimerStart (id, 10) == osOK);
    
    /* Wait for timer interval */
    for (i = 20; i; i--) {
      if (Tim_Var_Os > 0) {
        break;
      }
      osDelay(1);
    }
    ASSERT_TRUE (i != 0);
    ASSERT_TRUE (Tim_Var_Os > 0);
    
    /* Wait another interval */
    for (i = 20; i; i--) {
      if (Tim_Var_Os > 1) {
        break;
      }
      osDelay(1);
    }
    ASSERT_TRUE (i == 0);
    ASSERT_TRUE (Tim_Var_Os == 1);

    /* - Ensure correct status reporting */
    ASSERT_TRUE (osTimerStop (id) == osErrorResource);
    /* - Delete the timer */
    ASSERT_TRUE (osTimerDelete (id) == osOK);
  }
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief Test case: TC_TimerPeriodic
\details
- Create a periodic timer
- Ensure timer is not running
- Start timer and check if callback function is called more than once
- Stop the timer
- Ensure correct status reporting
- Delete the timer
*/
void TC_TimerPeriodic (void) {
  uint32_t i, var = 1;
  osTimerId_t id;

  /* - Create a periodic timer */
  id = osTimerNew (TimCb_Periodic, osTimerPeriodic, &var, NULL);
  ASSERT_TRUE (id != NULL);

  if (id) {
    /* - Ensure timer is not running */
    ASSERT_TRUE (osTimerStop (id) == osErrorResource);
    /* - Start timer and check if callback function is called more than once */
    ASSERT_TRUE (osTimerStart (id, 1000) == osOK);
    
    /* Wait for timer interval */
    for (i = 40; i; i--) {
      if (Tim_Var_Per > 2) {
        break;
      }
      // osDelay(1);
    }
    ASSERT_TRUE (i != 0);
    ASSERT_TRUE (Tim_Var_Per > 2);
    
    /* - Stop the timer */
    ASSERT_TRUE (osTimerStop (id) == osOK);
    /* - Ensure correct status reporting */
    ASSERT_TRUE (osTimerStop (id) == osErrorResource);
    /* - Delete the timer */
    ASSERT_TRUE (osTimerDelete (id) == osOK);
  }
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief Test case: TC_TimerParam
\details
- Test timer management functions with invalid parameters
*/
void TC_TimerParam (void) {
  osTimerId_t id = NULL;
  
  /* - Test invalid timer definition on osTimercreate */
  ASSERT_TRUE (osTimerNew (NULL, osTimerOnce, NULL, NULL) == NULL);
  
  /* - Test invalid timer id on osTimerStart */
  ASSERT_TRUE (osTimerStart (NULL, 0) == osErrorParameter);
  
  /* - Test invalid timer id on osTimerStop */
  ASSERT_TRUE (osTimerStop (NULL)   == osErrorParameter);
  
  /* - Test invalid timer id on osTimerDelete */
  ASSERT_TRUE (osTimerDelete (NULL) == osErrorParameter);

  /* - Test uninitialized parameter on osTimerStart */
  ASSERT_TRUE (osTimerStart (id, 0) == osErrorParameter);
  
  /* - Test uninitialized parameter on osTimerStop */
  ASSERT_TRUE (osTimerStop  (id)    == osErrorParameter);
}

/**
@}
*/ 
// end of group timer_func

/*-----------------------------------------------------------------------------
 * End of file
 *----------------------------------------------------------------------------*/
