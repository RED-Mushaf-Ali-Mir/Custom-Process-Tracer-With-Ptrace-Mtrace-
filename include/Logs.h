#ifndef LOGS_H
#define LOGS_H

#include <sys/types.h>
#include <stdio.h>
#include <time.h>

void init_logger(pid_t pid);
void log_syscall(long syscall_number, const char *syscall_name);
void close_logger();

#endif