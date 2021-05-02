/* MDH WCET BENCHMARK SUITE. File version $Id: lcdnum.c,v 1.3 2005/11/11 10:31:13 ael01 Exp $ */

/***********************************************************************
 *  FILE: lcdnum.c
 *
 *  PURPOSE: demonstrate effect of flow facts for straight loops
 *
 *  IDEA: reading from an in port mapped to a ten-item buffer,
 *        send first five characters to an LCD as numbers
 *
 ***********************************************************************/


 /*
  * Changes: JG 2005/12/23: Changed type of main to int Inserted prototypes.
                            Indented program.
  */

unsigned char   num_to_lcd(unsigned char a);

unsigned char 
num_to_lcd(unsigned char a)
{
	/*
	 * -0-            1            01 1   2         2   4        02  04
	 * -3-    i.e.    8     i.e.   08 4   5        16   32       10  20
	 * -6-           64            40
	 * 
	 */
	switch (a) {
		case 0x00:return 0;
	case 0x01:
		return 0x24;
	case 0x02:
		return 1 + 4 + 8 + 16 + 64;
	case 0x03:
		return 1 + 4 + 8 + 32 + 64;
	case 0x04:
		return 2 + 4 + 8 + 32;
	case 0x05:
		return 1 + 4 + 8 + 16 + 64;
	case 0x06:
		return 1 + 2 + 8 + 16 + 32 + 64;
	case 0x07:
		return 1 + 4 + 32;
	case 0x08:
		return 0x7F;	/* light all */
	case 0x09:
		return 0x0F + 32 + 64;
	case 0x0A:
		return 0x0F + 16 + 32;
	case 0x0B:
		return 2 + 8 + 16 + 32 + 64;
	case 0x0C:
		return 1 + 2 + 16 + 64;
	case 0x0D:
		return 4 + 8 + 16 + 32 + 64;
	case 0x0E:
		return 1 + 2 + 8 + 16 + 64;
	case 0x0F:
		return 1 + 2 + 8 + 16;
	}
	return 0;
}

volatile unsigned char IN;
volatile unsigned char OUT;

int 
lcdnum_main(void)
{
	int             i;
	unsigned char   a;

	for (i = 0; i < 10; i++) {
		a = IN;		/* scan port */
		if (i < 5) {
			a = a & 0x0F;
			OUT = num_to_lcd(a);
		}
	}
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
	hwthread_start(1, lcdnum_main, NULL);
	hwthread_start(2, lcdnum_main, NULL);
	hwthread_start(3, lcdnum_main, NULL);
	hwthread_start(4, lcdnum_main, NULL);
	hwthread_start(5, lcdnum_main, NULL);
	hwthread_start(6, lcdnum_main, NULL);
    hwthread_start(7, lcdnum_main, NULL);
	uint32_t slots[8] = { SLOT_T0, SLOT_T1, SLOT_T2, SLOT_T3, SLOT_T4, SLOT_T5, SLOT_T6,SLOT_T7 };
	uint32_t tmodes[8] = { TMODE_HA, TMODE_HA, TMODE_HA, TMODE_HA, TMODE_HA, TMODE_HA, TMODE_HA,TMODE_HA };
    set_slots(slots); // tid 0, 1, 2 round robin
    set_tmodes(tmodes); // all hard+active
    while((hwthread_done(1) & hwthread_done(2) & hwthread_done(3) & hwthread_done(4) &
	       hwthread_done(5) & hwthread_done(6) & hwthread_done(7)) == 0);
    return 0;

}