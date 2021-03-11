#include "cmsis_os2.h"
#include "flexpret_timing.h"
//  ==== Timer Management Functions ====

osTimerId_t osTimerNew (osTimerFunc_t func, osTimerType_t type, void *argument, const osTimerAttr_t *attr) {
    return NULL;
}
 
const char *osTimerGetName (osTimerId_t timer_id) {
    return "";
}
 
osStatus_t osTimerStart (osTimerId_t timer_id, uint32_t ticks) {
    return osOK;
}
 
osStatus_t osTimerStop (osTimerId_t timer_id) {
    return osOK;
}
 
uint32_t osTimerIsRunning (osTimerId_t timer_id) {
    return 0;
}
 
osStatus_t osTimerDelete (osTimerId_t timer_id) {
    return osOK;
}
 