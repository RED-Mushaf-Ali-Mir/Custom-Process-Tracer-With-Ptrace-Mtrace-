#ifndef TUI_H
#define TUI_H

#include <sys/types.h>

void do_tracer (pid_t tracee_pid,unsigned int);
pid_t pidByname(char *p_name);

#endif