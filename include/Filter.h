#ifndef FILTER_H
#define FILTER_H

/* Indices into the category table */
#define CAT_FILE_IO   0
#define CAT_PROCESS   1
#define CAT_MEMORY    2
#define CAT_IPC       3
#define CAT_SIGNAL    4
#define CAT_TIME      5
#define CAT_SECURITY  6
#define CAT_SCHEDULER 7
#define CAT_COUNT     8   


typedef struct {
    const char *name;
    const char **syscalls;
} SyscallCategory;

/* Gloabla Table*/
extern const SyscallCategory categories[CAT_COUNT];

/*
 * Returns 1 if syscall_name belongs to ANY category whose bit is set
 * in the bitmask `filter`  (bit N = 1 << N, matching CAT_* defines).
 * If filter == 0 (nothing selected) every syscall passes through.
 */
int filter_syscall(const char *syscall_name, unsigned int filter);

#endif 