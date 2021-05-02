/* MDH WCET BENCHMARK SUITE. File version $Id: loop3.c,v 1.1 2005/11/11 10:17:03 ael01 Exp $ */


/* Changes:
 * JG 2005/12/12: Indented program. Removed argument to main.
 */
 
#define PLOOP(A,B,I) do                \
{                                      \
  for (i = (A); i <  (B); i += (I))    \
    s += i;                            \
                                       \
  for (i = (A); i <= (B); i += (I))    \
    s += i;                            \
                                       \
  for (i = (A); i <  (B); i -= (-(I))) \
    s += i;                            \
                                       \
  for (i = (A); i <= (B); i -= (-(I))) \
    s += i;                            \
} while (0)

#define MLOOP(A,B,D) do                \
{                                      \
  for (i = (A); i >  (B); i -= (D))    \
    s += i;                            \
                                       \
  for (i = (A); i >= (B); i -= (D))    \
    s += i;                            \
                                       \
  for (i = (A); i >  (B); i += (-(D))) \
    s += i;                            \
                                       \
  for (i = (A); i >= (B); i += (-(D))) \
    s += i;                            \
} while (0)


int 
loop3_main(int argc)
{
	int             s = 0;
	int             i;
	argc = 2;


	PLOOP(0, 3, 1);
	PLOOP(1, 2, 1);
	PLOOP(-3, -2, 1);
	PLOOP(-4, -1, 1);
	PLOOP(-5, 0, 1);
	PLOOP(-6, 4, 1);

	PLOOP(0, 3, argc);
	PLOOP(1, 2, argc);
	PLOOP(-3, -2, argc);
	PLOOP(-4, -1, argc);
	PLOOP(-5, 0, argc);
	PLOOP(-6, 4, argc);

	MLOOP(5, 2, 1);
	MLOOP(6, 1, 1);
	MLOOP(7, 0, 1);
	MLOOP(8, -1, 1);
	MLOOP(9, -2, 1);
	MLOOP(0, -3, 1);
	MLOOP(-1, -5, 1);
	MLOOP(-2, -7, 1);
	MLOOP(16, -8, 1);

	MLOOP(5, 2, argc);
	MLOOP(6, 1, argc);
	MLOOP(7, 0, argc);
	MLOOP(8, -1, argc);
	MLOOP(9, -2, argc);
	MLOOP(0, -3, argc);
	MLOOP(-1, -5, argc);
	MLOOP(-2, -7, argc);
	MLOOP(16, -8, argc);

	return s;
}


#include "flexpret_threads.h"
#define THREADS 8
extern volatile hwthread_state startup_state[THREADS];

void hwthread_start(uint32_t tid, void (*func)(), uint32_t stack_address) {
    startup_state[tid].func = func;
    if(stack_address != NULL) {
        startup_state[tid].stack_address = stack_address;
    }
}

uint32_t hwthread_done(uint32_t tid) {
    return (startup_state[tid].func == NULL);
}

int main() {
	hwthread_start(1, loop3_main, NULL);
	hwthread_start(2, loop3_main, NULL);
	hwthread_start(3, loop3_main, NULL);
	hwthread_start(4, loop3_main, NULL);
	hwthread_start(5, loop3_main, NULL);
	hwthread_start(6, loop3_main, NULL);
    hwthread_start(7, loop3_main, NULL);
	uint32_t slots[8] = { SLOT_T0, SLOT_T1, SLOT_T2, SLOT_T3, SLOT_T4, SLOT_T5, SLOT_T6,SLOT_T7 };
	uint32_t tmodes[8] = { TMODE_HA, TMODE_HA, TMODE_HA, TMODE_HA, TMODE_HA, TMODE_HA, TMODE_HA,TMODE_HA };
    set_slots(slots); // tid 0, 1, 2 round robin
    set_tmodes(tmodes); // all hard+active
    while((hwthread_done(1) & hwthread_done(2) & hwthread_done(3) & hwthread_done(4) &
	       hwthread_done(5) & hwthread_done(6) & hwthread_done(7)) == 0);
    return 0;

}