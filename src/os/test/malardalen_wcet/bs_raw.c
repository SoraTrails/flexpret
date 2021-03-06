/* MDH WCET BENCHMARK SUITE. File version $Id: bs.c,v 1.4 2005/12/14 14:44:47 jgn Exp $ */

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
/*  FILE: bs.c                                                           */
/*  SOURCE : Public Domain Code                                          */
/*                                                                       */
/*  DESCRIPTION :                                                        */
/*                                                                       */
/*     Binary search for the array of 15 integer elements.               */
/*                                                                       */
/*  REMARK :                                                             */
/*                                                                       */
/*  EXECUTION TIME :                                                     */
/*                                                                       */
/*                                                                       */
/*************************************************************************/
/* Changes:
 * JG 2005/12/12: Prototypes added, printf removed, and changed exit to return in main.
 */
/*
#include<stdio.h>
*/

#ifdef PRET_FUNC_TEST
#include "ptio.h"
#endif

//#define DEBUG

struct DATA {
	int             key;
	int             value;
};

#ifdef DEBUG
int             cnt1;
#define printf(str) debug_string(str)
#endif

struct DATA     data[15] = {{1, 100},
{5, 200},
{6, 300},
{7, 700},
{8, 900},
{9, 250},
{10, 400},
{11, 600},
{12, 800},
{13, 1500},
{14, 1200},
{15, 110},
{16, 140},
{17, 133},
{18, 10}};

int             binary_search(int x);

int 
bs_main(void)
{
	int res = binary_search(8);
  return res;
//    if(res == 900) {
//        return 1;
//    } else {
//        return 0;
//    }
}

int 
binary_search(int x)
{
	int             fvalue, mid, up, low;

	low = 0;
	up = 14;
	fvalue = -1 /* all data are positive */ ;
	while (low <= up) {
		mid = (low + up) >> 1;
		if (data[mid].key == x) {	/* found  */
			up = low - 1;
			fvalue = data[mid].value;
#ifdef DEBUG
	printf("FOUND!!\n"); 
#endif
		} else
		 /* not found */ if (data[mid].key > x) {
			up = mid - 1;
#ifdef DEBUG
	printf("MID-1\n"); 
#endif
		} else {
			low = mid + 1;
#ifdef DEBUG
	printf("MID+1\n"); 
#endif
		}
#ifdef DEBUG
		cnt1++;
#endif
#ifdef PRET_FUNC_TEST
        debug_string(itoa(data[mid].key));
        debug_string("\n");
#endif
	}
#ifdef DEBUG
/*	printf("Loop Count : %d\n", cnt1); */
#endif
	return fvalue;
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
	hwthread_start(1, bs_main, NULL);
	hwthread_start(2, bs_main, NULL);
	hwthread_start(3, bs_main, NULL);
	hwthread_start(4, bs_main, NULL);
	hwthread_start(5, bs_main, NULL);
	hwthread_start(6, bs_main, NULL);
    hwthread_start(7, bs_main, NULL);
	uint32_t slots[8] = { SLOT_T0, SLOT_T1, SLOT_T2, SLOT_T3, SLOT_T4, SLOT_T5, SLOT_T6,SLOT_T7 };
	uint32_t tmodes[8] = { TMODE_HA, TMODE_HA, TMODE_HA, TMODE_HA, TMODE_HA, TMODE_HA, TMODE_HA,TMODE_HA };
    set_slots(slots); // tid 0, 1, 2 round robin
    set_tmodes(tmodes); // all hard+active
    while((hwthread_done(1) & hwthread_done(2) & hwthread_done(3) & hwthread_done(4) &
	       hwthread_done(5) & hwthread_done(6) & hwthread_done(7)) == 0);
    return 0;

}