FlexPRET OS
================================================================================
FlexPRET OS is a lightweight rtos implementation based on [FlexPRET](https://github.com/pretis/flexpret) processor, which implements system initailization, thread management, timer management, mutex management, trap management and some self-defined APIs.

Before getting started, you may refer to the depository of [FlexPRET](https://github.com/pretis/flexpret) to know what FlexPRET is.

FlexPRET OS is based on [this commit](https://github.com/pretis/flexpret/commit/446635af19f2ba54444da54afa49cba1a1c6d28d) of FlexPRET, which is a chisel2 version.

The most API of FlexPRET OS refers to [CMSIS RTOS v2](https://www.keil.com/pack/doc/cmsis/RTOS2/html/index.html).


Software Implementation
--------------------------------------------------------------------------------
### Kernel Information and Control
| API |
| --  |
| osKernelInitialize |
| osKernelGetInfo |
| osKernelGetState |
| osKernelStart |
| osKernelGetTickCount |
| osKernelGetTickFreq |

### Thread Management
| API |
| --  |
|osThreadNew|
|osThreadGetName|
|osThreadGetId|
|osThreadGetState|
|osThreadGetStackSize|
|osThreadGetStackSpace|
|osThreadSetPriority|
|osThreadGetPriority|
|osThreadSuspend|
|osThreadResume|
|osThreadDetach|
|osThreadJoin|
|osThreadExit|
|osThreadTerminate|
|osThreadGetCount|
|osThreadEnumerate|
|osDelay|
|osDelayUntil|

### Timer Management
| API |
| --  |
|osTimerNew|
|osTimerGetName|
|osTimerStart|
|osTimerStop|
|osTimerIsRunning|
|osTimerDelete|

### Mutex Management
| API |
| --  |
|osMutexNew|
|osMutexGetName|
|osMutexAcquire|
|osMutexRelease|
|osMutexGetOwner|
|osMutexDelete|
### Trap Management

### Self-defined APIs
| API |
| --  |
|osSchedulerGetFreq|
|osSchedulerSetSlotNum|
|osSchedulerSetSoftSlotNum|
|osSchedulerGetSRTTNum|
|osSchedulerGetTmodes|
|osSchedulerSetTmodes|
|osThreadSetTrapHandler|
|osThreadGetTrapHandler|
|osThreadJoinAll|
|osThreadGetTimer|
|osTimerSetFunc|
|osMutexSetSpin|
|osMuteGetSpin|


Timing Instruction Extension
--------------------------------------------------------------------------------
FlexPRET OS extends FlexPRET's timing instruction:
| Pseudo-instruction	| Implementation	| Semantics |
| ---	| --- | --- |
| mt |	custom3 0, r1, 0, 0 | Mark the beginning of the code block, and give the execution deadline (stored in register) of the code block |
| mti |	lui zero, imm	| Mark the beginning of the code block, and give the execution deadline (stored in 24bit immediate) of the code block |
| fd	| custom3 zero, zero, zero, 0	| Mark the end of the code block. When the execution time of the code block exceeds the agreed time of the mt/mti instruction, mtfd exception will be raised |

The mtfd instruction is mentioned in the paper:
- D. Broman, M. Zimmer, Y. Kim, H. Kim, J. Cai, A. Shrivastava, S. A. Edwards, and E. A. Lee, [Precision timed infrastructure: Design challenges](http://ieeexplore.ieee.org/abstract/document/6573221/) in Proceedings of the Electronic System Level Synthesis Conference (ESLsyn), May 2013, pp. 1–6.

The above timing instruction is implemented both in hardware and GCC. GCC implementation is based on GCC 4.9.2, which is the version of riscv-gun-toolchain that FlexPRET currently supports. Corresponding patch is in `gcc_patch` directory.

Note that only branch structure is supported currently in GCC implementation. When there is a while/for between mt and fd, the static WCET calculation will be aborted.  When there is a function call between mt and fd, the excution time of the function will not be accumulated into the WCET result.

When mtfd exception is followed by an interrupt or exception, mtfd exception will be ignored.

Other
--------------------------------------------------------------------------------

Any issue about FlexPRET OS is welcome.

Known Bugs & TODO
--------------------------------------------------------------------------------
TBD
