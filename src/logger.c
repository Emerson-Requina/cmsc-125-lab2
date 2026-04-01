#include <stdio.h>
#include "logger.h"

void log_step(int time, const char* pid, const char* status) {
    printf("[TIME %03d] Process %-5s: %s\n", time, pid, status);
}

void log_arrival(int time, const char* pid) {
    printf("[TIME %03d] ARRIVAL      : Process %s arrived\n", time, pid);
}

void log_preemption(int time, const char* old_pid, const char* new_pid) {
    printf("[TIME %03d] PREEMPTION   : %s yielded to %s\n", time, old_pid, new_pid);
}

void log_gantt(const char* pid, int start, int end) {
    printf("Gantt: %s [%d - %d]\n", pid, start, end);
}