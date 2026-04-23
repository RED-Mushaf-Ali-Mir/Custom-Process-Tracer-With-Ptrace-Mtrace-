#include "Logs.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

static FILE *log_file = NULL;

void init_logger(pid_t pid) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    char filename[256];
    strftime(filename, sizeof(filename), "trace_%Y%m%d_%H%M%S.log", t);

    log_file = fopen(filename, "w");
    if (!log_file) {
        perror("Failed to create log file");
        exit(1);
    }

    fprintf(log_file, "=== Syscall Trace Session ===\n");
    fprintf(log_file, "PID: %d\n", pid);
    fprintf(log_file, "Started: %s", ctime(&now));
    fprintf(log_file, "=============================\n\n");

    printf("\n[Logger] Logging to: %s\n", filename);
}

void log_syscall(long syscall_number, const char *syscall_name) {
    if (!log_file) return;

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char timestamp[64];
    strftime(timestamp, sizeof(timestamp), "%H:%M:%S", t);

    fprintf(log_file, "[%s] syscall #%-4ld  %s\n",
            timestamp, syscall_number, syscall_name);

    fflush(log_file);
}

void close_logger() {
    if (log_file) {
        fprintf(log_file, "\n=== Session Ended ===\n");
        fclose(log_file);
        log_file = NULL;
    }
}