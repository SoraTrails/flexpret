#ifndef FLEXPRET_KERNEL_H
#define FLEXPRET_KERNEL_H

#define KERNEL_VER_MAJOR(ver) (((ver) >> 16) & 0xFF)
#define KERNEL_VER_MINOR(ver) (((ver) >> 8) & 0xFF)
#define KERNEL_VER_PATCHLEVEL(ver) ((ver) & 0xFF)
#define KERNEL_VER(v1, v2, v3) (((0xFF & v1) << 16) & ((0xFF & v2) << 8) & (0xFF & v3))

#ifndef FLEXPRET_OS_VERSION
#define FLEXPRET_OS_VERSION KERNEL_VER(1, 0, 0)
#endif

#endif