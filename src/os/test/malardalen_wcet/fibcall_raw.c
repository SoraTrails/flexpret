/* MDH WCET BENCHMARK SUITE. File version $Id: fibcall.c,v 1.3 2005/11/11 10:30:19 ael01 Exp $ */

/*************************************************************************/
/*                                                                       */
/*   SNU-RT Benchmark Suite for Worst Case Timing Analysis               */
/*   =====================================================               */
/*                              Collected and Modified by S.-S. Lim      */
/*                                           sslim@archi.snu.ac.kr       */
/*                                         Real-Time Research Group      */
/*                                        Seoul National University      */
/*                                                                       */
/*                                                                       */
/*        < Features > - restrictions for our experimental environment   */
/*                                                                       */
/*          1. Completely structured.                                    */
/*               - There are no unconditional jumps.                     */
/*               - There are no exit from loop bodies.                   */
/*                 (There are no 'break' or 'return' in loop bodies)     */
/*          2. No 'switch' statements.                                   */
/*          3. No 'do..while' statements.                                */
/*          4. Expressions are restricted.                               */
/*               - There are no multiple expressions joined by 'or',     */
/*                'and' operations.                                      */
/*          5. No library calls.                                         */
/*               - All the functions needed are implemented in the       */
/*                 source file.                                          */
/*                                                                       */
/*                                                                       */
/*************************************************************************/
/*                                                                       */
/*  FILE: fibcall.c                                                      */
/*  SOURCE : Public Domain Code                                          */
/*                                                                       */
/*  DESCRIPTION :                                                        */
/*                                                                       */
/*     Summing the Fibonacci series.                                     */
/*                                                                       */
/*  REMARK :                                                             */
/*                                                                       */
/*  EXECUTION TIME :                                                     */
/*                                                                       */
/*                                                                       */
/*************************************************************************/

 /*
  * Changes: JG 2005/12/21: Inserted prototypes.
  *                         Indented program.
  */

int             fib(int n);

int 
fib(int n)
{
	int             i, Fnew, Fold, temp, ans;

	Fnew = 1;
	Fold = 0;
	for (i = 2;
	     i <= 30 && i <= n;	/* apsim_loop 1 0 */
	     i++) {
		temp = Fnew;
		Fnew = Fnew + Fold;
		Fold = temp;
	}
	ans = Fnew;
	return ans;
}

int 
fibcall_main()
{
	int             a;

	a = 30;
	fib(a);
	return a;
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
	hwthread_start(1, fibcall_main, NULL);
	hwthread_start(2, fibcall_main, NULL);
	hwthread_start(3, fibcall_main, NULL);
	hwthread_start(4, fibcall_main, NULL);
	hwthread_start(5, fibcall_main, NULL);
	hwthread_start(6, fibcall_main, NULL);
    hwthread_start(7, fibcall_main, NULL);
	uint32_t slots[8] = { SLOT_T0, SLOT_T1, SLOT_T2, SLOT_T3, SLOT_T4, SLOT_T5, SLOT_T6,SLOT_T7 };
	uint32_t tmodes[8] = { TMODE_HA, TMODE_HA, TMODE_HA, TMODE_HA, TMODE_HA, TMODE_HA, TMODE_HA,TMODE_HA };
    set_slots(slots); // tid 0, 1, 2 round robin
    set_tmodes(tmodes); // all hard+active
    while((hwthread_done(1) & hwthread_done(2) & hwthread_done(3) & hwthread_done(4) &
	       hwthread_done(5) & hwthread_done(6) & hwthread_done(7)) == 0);
    return 0;

}