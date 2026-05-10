

#include "Ptracer.h"
#include "Filter.h"       

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>       /* raw-mode helpers                  */
#include <unistd.h>
#include <errno.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <sys/select.h>    /* select() for non-blocking key check */

#include "SysCalls.h"
#include "Logs.h"

/* ── raw-mode helpers (so 'q' is seen without Enter) ─────── */

static struct termios orig_termios;

static void enable_raw_mode(void)
{
    tcgetattr(STDIN_FILENO, &orig_termios);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO);   /* no line buffer, no echo */
    raw.c_cc[VMIN]  = 0;               /* non-blocking read        */
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
}

static void disable_raw_mode(void)
{
    tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
}

/*
 * Returns 1 if the user has pressed 'q' (non-blocking, no Enter needed).
 * Returns 0 otherwise.
 */
static int user_quit(void)
{
    fd_set fds;
    struct timeval tv = {0, 0};   /* zero timeout */

    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);

    if (select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) > 0) {
        char c = 0;
        if (read(STDIN_FILENO, &c, 1) == 1 && (c == 'q' || c == 'Q'))
            return 1;
    }
    return 0;
}



void do_tracer(pid_t  tracee_pid, unsigned int filter)
{
    int  status;
    struct user_regs_struct regs;

    if (ptrace(PTRACE_ATTACH, tracee_pid, NULL, NULL) == -1) {
        perror("ptrace PTRACE_ATTACH");
        return;
    }

    printf("\nTracee attached successfully (PID %d)", (int)tracee_pid);
    printf("\nPress 'q' at any time to stop tracing.\n");
    fflush(stdout);

    waitpid(tracee_pid, &status, 0);

    if (!WIFSTOPPED(status)) {
        printf("\nProcess did not stop as expected.\n");
        return;
    }

    printf("Process stopped. Beginning trace...\n\n");
    fflush(stdout);

    init_logger(tracee_pid);
    enable_raw_mode();

    while (1) {
        /* ── check for 'q' before each syscall ── */
        if (user_quit()) {
            printf("\n[q pressed] Stopping tracer.\n");
            break;
        }

        if (ptrace(PTRACE_GETREGS, tracee_pid, NULL, &regs) == -1) {
            perror("ptrace GETREGS");
            break;
        }

        /* aply filter */
        const char *name = sysCallName(regs.orig_rax);

        if (filter_syscall(name, filter)) {
            printf("\nTrace Systemcall : %lld  (%s)",
                   regs.orig_rax, name);
            log_syscall(regs.orig_rax, name);
        }

        /* step to next syscall*/
        ptrace(PTRACE_SYSCALL, tracee_pid, NULL, NULL);
        waitpid(tracee_pid, &status, 0);

        if (WIFEXITED(status)) {
            printf("\nChild exited with status %d\n", WEXITSTATUS(status));
            break;
        }
    }

    disable_raw_mode();
    close_logger();

    /* Detach */
    ptrace(PTRACE_DETACH, tracee_pid, NULL, NULL);
}