#include "cmsis_os2.h"
#include "flexpret_os.h"

extern int bs_main(void);
extern int cover_main(void);
extern int duff_main(void);
extern int fibcall_main(void);
extern int insertsort_main(void);
extern int lcdnum_main(void);
extern int loop3_main(void);
extern int ns_main(void);

// // os schedule
// int main() {
// 	const osThreadAttr_t attr[7] = {
// 		{ "FlexpretThread", osThreadJoinable, NULL, sizeof(hwthread_state), NULL, 0, osPriorityRealtime, 0, 0},
// 		{ "FlexpretThread", osThreadJoinable, NULL, sizeof(hwthread_state), NULL, 0, osPriorityRealtime, 0, 0},
// 		{ "FlexpretThread", osThreadJoinable, NULL, sizeof(hwthread_state), NULL, 0, osPriorityRealtime, 0, 0},
// 		{ "FlexpretThread", osThreadJoinable, NULL, sizeof(hwthread_state), NULL, 0, osPriorityRealtime, 0, 0},
// 		{ "FlexpretThread", osThreadJoinable, NULL, sizeof(hwthread_state), NULL, 0, osPriorityRealtime, 0, 0},
// 		{ "FlexpretThread", osThreadJoinable, NULL, sizeof(hwthread_state), NULL, 0, osPriorityRealtime, 0, 0},
// 		{ "FlexpretThread", osThreadJoinable, NULL, sizeof(hwthread_state), NULL, 0, osPriorityRealtime, 0, 0}
// 	};

// 	osKernelInitialize();
// 	osThreadId_t th[7];
// 	register int i;
// 	// th[0] = osThreadNew(bs_main, NULL, &attr[0]);
// 	th[0] = osThreadNew(cover_main, NULL, &attr[0]);
// 	th[1] = osThreadNew(duff_main, NULL, &attr[1]);
// 	th[2] = osThreadNew(fibcall_main, NULL, &attr[2]);
// 	th[3] = osThreadNew(insertsort_main, NULL, &attr[3]);
// 	th[4] = osThreadNew(lcdnum_main, NULL, &attr[4]);
// 	th[5] = osThreadNew(loop3_main, NULL, &attr[5]);
// 	th[6] = osThreadNew(ns_main, NULL, &attr[6]);
// 	osKernelStart();
// 	osThreadJoinAll(th, 7);
// 	// for (i = 0; i < 7; i++) {
// 	// 	osThreadJoin(th[i]);
// 	// }
// 	return 0;
// }


// // raw schedule
// #include "flexpret_threads.h"
// #define THREADS 8
// extern volatile hwthread_state startup_state[THREADS];

// void hwthread_start(uint32_t tid, void (*func)(), uint32_t stack_address) {
//     startup_state[tid].func = func;
//     if(stack_address != NULL) {
//         startup_state[tid].stack_address = stack_address;
//     }
// }

// uint32_t hwthread_done(uint32_t tid) {
//     return (startup_state[tid].func == NULL);
// }

// int main() {
// 	// hwthread_start(1, bs_main, NULL);
// 	hwthread_start(1, cover_main, NULL);
// 	hwthread_start(2, duff_main, NULL);
// 	hwthread_start(3, fibcall_main, NULL);
// 	hwthread_start(4, insertsort_main, NULL);
// 	hwthread_start(5, lcdnum_main, NULL);
//     hwthread_start(6, loop3_main, NULL);
//     hwthread_start(7, ns_main, NULL);
	
// 	uint32_t slots[8] = { SLOT_T0, SLOT_T1, SLOT_T2, SLOT_T3, SLOT_T4, SLOT_T5, SLOT_T6,SLOT_T7 };
// 	uint32_t tmodes[8] = { TMODE_HA, TMODE_HA, TMODE_HA, TMODE_HA, TMODE_HA, TMODE_HA, TMODE_HA,TMODE_HA };
//     set_slots(slots); // tid 0, 1, 2 round robin
//     set_tmodes(tmodes); // all hard+active
//     while((hwthread_done(1) & hwthread_done(2) & hwthread_done(3) & hwthread_done(4) &
// 	       hwthread_done(5) & hwthread_done(6) & hwthread_done(7)) == 0);
//     return 0;

// }


int main() {
    osKernelInitialize();
    osKernelStart();
    // osDelay(0x7ffffffc);
    gpo_set_0(0xffffffff);
    gpo_set_1(0xffffffff);
    gpo_set_2(0xffffffff);
    gpo_set_3(0xffffffff);
}