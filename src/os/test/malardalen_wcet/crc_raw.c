/* MDH WCET BENCHMARK SUITE. File version $Id: crc.c,v 1.4 2006/01/27 13:15:05 jgn Exp $ */

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
/*  FILE: crc.c                                                          */
/*  SOURCE : Numerical Recipes in C - The Second Edition                 */
/*                                                                       */
/*  DESCRIPTION :                                                        */
/*                                                                       */
/*     A demonstration for CRC (Cyclic Redundancy Check) operation.      */
/*     The CRC is manipulated as two functions, icrc1 and icrc.          */
/*     icrc1 is for one character and icrc uses icrc1 for a string.      */
/*     The input string is stored in array lin[].                        */
/*     icrc is called two times, one for X-Modem string CRC and the      */
/*     other for X-Modem packet CRC.                                     */
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

typedef unsigned char uchar;
#define LOBYTE(x) ((uchar)((x) & 0xFF))
#define HIBYTE(x) ((uchar)((x) >> 8))

unsigned char   lin[256] = "asdffeagewaHAFEFaeDsFEawFdsFaefaeerdjgp";

unsigned short  icrc1(unsigned short crc, unsigned char onech);
unsigned short 
icrc(unsigned short crc, unsigned long len,
     short jinit, int jrev);

unsigned short 
icrc1(unsigned short crc, unsigned char onech)
{
	int             i;
	unsigned short  ans = (crc ^ onech << 8);

	for (i = 0; i < 8; i++) {
		if (ans & 0x8000)
			ans = (ans <<= 1) ^ 4129;
		else
			ans <<= 1;
	}
	return ans;
}

unsigned short 
icrc(unsigned short crc, unsigned long len,
     short jinit, int jrev)
{
	unsigned short  icrc1(unsigned short crc, unsigned char onech);
	static unsigned short icrctb[256], init = 0;
	static uchar    rchr[256];
	unsigned short  tmp1, tmp2, j, cword = crc;
	static uchar    it[16] = {0, 8, 4, 12, 2, 10, 6, 14, 1, 9, 5, 13, 3, 11, 7, 15};

	if (!init) {
		init = 1;
		for (j = 0; j <= 255; j++) {
			icrctb[j] = icrc1(j << 8, (uchar) 0);
			rchr[j] = (uchar) (it[j & 0xF] << 4 | it[j >> 4]);
		}
	}
	if (jinit >= 0)
		cword = ((uchar) jinit) | (((uchar) jinit) << 8);
	else if (jrev < 0)
		cword = rchr[HIBYTE(cword)] | rchr[LOBYTE(cword)] << 8;
#ifdef DEBUG
	printf("len = %d\n", len);
#endif
	for (j = 1; j <= len; j++) {
		if (jrev < 0) {
			tmp1 = rchr[lin[j]] ^ HIBYTE(cword);
		} else {
			tmp1 = lin[j] ^ HIBYTE(cword);
		}
		cword = icrctb[tmp1] ^ LOBYTE(cword) << 8;
	}
	if (jrev >= 0) {
		tmp2 = cword;
	} else {
		tmp2 = rchr[HIBYTE(cword)] | rchr[LOBYTE(cword)] << 8;
	}
	return (tmp2);
}


int 
crc_main(void)
{

	unsigned short  i1, i2;
	unsigned long   n;

	n = 40;
	lin[n + 1] = 0;
	i1 = icrc(0, n, (short) 0, 1);
	lin[n + 1] = HIBYTE(i1);
	lin[n + 2] = LOBYTE(i1);
	i2 = icrc(i1, n + 2, (short) 0, 1);
	return 0;
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
	hwthread_start(1, crc_main, NULL);
	hwthread_start(2, crc_main, NULL);
	hwthread_start(3, crc_main, NULL);
	hwthread_start(4, crc_main, NULL);
	hwthread_start(5, crc_main, NULL);
	hwthread_start(6, crc_main, NULL);
    hwthread_start(7, crc_main, NULL);
	uint32_t slots[8] = { SLOT_T0, SLOT_T1, SLOT_T2, SLOT_T3, SLOT_T4, SLOT_T5, SLOT_T6,SLOT_T7 };
	uint32_t tmodes[8] = { TMODE_HA, TMODE_HA, TMODE_HA, TMODE_HA, TMODE_HA, TMODE_HA, TMODE_HA,TMODE_HA };
    set_slots(slots); // tid 0, 1, 2 round robin
    set_tmodes(tmodes); // all hard+active
    while((hwthread_done(1) & hwthread_done(2) & hwthread_done(3) & hwthread_done(4) &
	       hwthread_done(5) & hwthread_done(6) & hwthread_done(7)) == 0);
    return 0;

}