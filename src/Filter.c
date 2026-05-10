#include "Filter.h"
#include <string.h>

/* ── syscall lists ────────────────────────────────────────── */

static const char *file_io_syscalls[] = {
    "read","write","open","close","stat","fstat","lstat",
    "pread64","pwrite64","readv","writev","lseek","access",
    "fsync","fdatasync","truncate","ftruncate","getdents",
    "getdents64","openat","newfstatat","unlink","unlinkat",
    "rename","renameat","renameat2","mkdir","mkdirat","rmdir",
    "creat","link","linkat","symlink","symlinkat","readlink",
    "readlinkat","chmod","fchmod","fchmodat","chown","fchown",
    "lchown","fchownat","statx",
    NULL
};

static const char *process_syscalls[] = {
    "clone","fork","vfork","execve","execveat","exit",
    "exit_group","wait4","waitid","kill","tgkill","tkill",
    "getpid","getppid","gettid",
    NULL
};

static const char *memory_syscalls[] = {
    "mmap","munmap","mprotect","brk","mremap","mlock",
    "munlock","mlockall","munlockall","madvise","mincore",
    "mbind","set_mempolicy","get_mempolicy","userfaultfd",
    "membarrier","mlock2","pkey_mprotect","pkey_alloc","pkey_free",
    NULL
};

static const char *ipc_syscalls[] = {
    "pipe","pipe2","shmget","shmat","shmctl","shmdt",
    "semget","semop","semctl","semtimedop","msgget","msgsnd",
    "msgrcv","msgctl","futex","eventfd","eventfd2",
    NULL
};

static const char *signal_syscalls[] = {
    "rt_sigaction","rt_sigprocmask","rt_sigreturn","rt_sigpending",
    "rt_sigtimedwait","rt_sigqueueinfo","rt_sigsuspend","sigaltstack",
    "signalfd","signalfd4","kill","tgkill","tkill","pause",
    NULL
};

static const char *time_syscalls[] = {
    "nanosleep","clock_gettime","clock_settime","clock_getres",
    "clock_nanosleep","gettimeofday","settimeofday","time","times",
    "alarm","getitimer","setitimer","timer_create","timer_settime",
    "timer_gettime","timer_delete",
    NULL
};

static const char *security_syscalls[] = {
    "setuid","setgid","setreuid","setregid","setresuid","setresgid",
    "capget","capset","seccomp","ptrace","prctl","arch_prctl",
    "setfsuid","setfsgid",
    NULL
};

static const char *scheduler_syscalls[] = {
    "sched_yield","sched_setparam","sched_getparam",
    "sched_setscheduler","sched_getscheduler","sched_setaffinity",
    "sched_getaffinity","sched_setattr","sched_getattr",
    "set_tid_address","rseq",
    NULL
};


const SyscallCategory categories[CAT_COUNT] = {
    [CAT_FILE_IO]   = { "File I/O",      file_io_syscalls   },
    [CAT_PROCESS]   = { "Process",       process_syscalls   },
    [CAT_MEMORY]    = { "Memory",        memory_syscalls    },
    [CAT_IPC]       = { "IPC",           ipc_syscalls       },
    [CAT_SIGNAL]    = { "Signals",       signal_syscalls    },
    [CAT_TIME]      = { "Time",          time_syscalls      },
    [CAT_SECURITY]  = { "Security",      security_syscalls  },
    [CAT_SCHEDULER] = { "Scheduler",     scheduler_syscalls },
};


/*
 * Returns 1 if syscall_name should be traced given the bitmask.
 * filter == 0 means "trace everything" (no filter applied).
 */
int filter_syscall(const char *syscall_name, unsigned int filter)
{
    if (filter == 0) return 1;   /* nothing selected = trace all */

    for (int cat = 0; cat < CAT_COUNT; cat++) {
        if (!(filter & (1u << cat))) continue;   /* category not selected */

        const char **list = categories[cat].syscalls;
        for (int i = 0; list[i] != NULL; i++) {
            if (strcmp(syscall_name, list[i]) == 0) return 1;
        }
    }
    return 0;
}