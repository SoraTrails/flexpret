#include "cmsis_os2.h"
#include "flexpret_threads.h"
#include "flexpret_io.h"

/*----------------------------------------------------------------------------
 * Application main thread
 *---------------------------------------------------------------------------*/
__NO_RETURN static void app_main (void *argument) {
  (void)argument;
  // ...
  for (;;) {}
}

int main() {
    char stack;
    debug_string(itoa_hex((uint32_t) &stack));
    debug_string("\n");

    osKernelInitialize (); 
    osThreadNew(app_main, NULL, NULL);
    osKernelStart (); 
    for (;;) {}
    return 0;
}