/* MDH WCET BENCHMARK SUITE. File version $Id: insertsort.c,v 1.3 2005/11/11 10:30:41 ael01 Exp $ */

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
/*  FILE: insertsort.c                                                   */
/*  SOURCE : Public Domain Code                                          */
/*                                                                       */
/*  DESCRIPTION :                                                        */
/*                                                                       */
/*     Insertion sort for 10 integer numbers.                            */
/*     The integer array a[] is initialized in main function.            */
/*                                                                       */
/*  REMARK :                                                             */
/*                                                                       */
/*  EXECUTION TIME :                                                     */
/*                                                                       */
/*                                                                       */
/*************************************************************************/

/* Changes:
 * JG 2005/12/12: Indented program.
 */

#ifdef DEBUG
int             cnt1, cnt2;
#endif

unsigned int    a[11];

int 
insertsort_main()
{
	int             i, j, temp;

	a[0] = 0;		/* assume all data is positive */
	a[1] = 11;
	a[2] = 10;
	a[3] = 9;
	a[4] = 8;
	a[5] = 7;
	a[6] = 6;
	a[7] = 5;
	a[8] = 4;
	a[9] = 3;
	a[10] = 2;
	i = 2;
	while (i <= 10) {
#ifdef DEBUG
		cnt1++;
#endif
		j = i;
#ifdef DEBUG
		cnt2 = 0;
#endif
		while (a[j] < a[j - 1]) {
#ifdef DEBUG
			cnt2++;
#endif
			temp = a[j];
			a[j] = a[j - 1];
			a[j - 1] = temp;
			j--;
		}
#ifdef DEBUG
		printf("Inner Loop Counts: %d\n", cnt2);
#endif
		i++;
	}
#ifdef DEBUG
	printf("Outer Loop : %d ,  Inner Loop : %d\n", cnt1, cnt2);
#endif
	return 1;
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
	hwthread_start(1, insertsort_main, NULL);
	hwthread_start(2, insertsort_main, NULL);
	hwthread_start(3, insertsort_main, NULL);
	hwthread_start(4, insertsort_main, NULL);
	hwthread_start(5, insertsort_main, NULL);
	hwthread_start(6, insertsort_main, NULL);
    hwthread_start(7, insertsort_main, NULL);
	uint32_t slots[8] = { SLOT_T0, SLOT_T1, SLOT_T2, SLOT_T3, SLOT_T4, SLOT_T5, SLOT_T6,SLOT_T7 };
	uint32_t tmodes[8] = { TMODE_HA, TMODE_HA, TMODE_HA, TMODE_HA, TMODE_HA, TMODE_HA, TMODE_HA,TMODE_HA };
    set_slots(slots); // tid 0, 1, 2 round robin
    set_tmodes(tmodes); // all hard+active
    while((hwthread_done(1) & hwthread_done(2) & hwthread_done(3) & hwthread_done(4) &
	       hwthread_done(5) & hwthread_done(6) & hwthread_done(7)) == 0);
    return 0;

}