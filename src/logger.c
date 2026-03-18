#include <stdio.h>
#include "../include/logger.h"

// void log_step(int time, const char* pid, const char* status) {
//     printf("[TIME %d] Process %s: %s\n", time, pid, status);
// }
void log_step(int time, const char* pid, const char* status) {
    printf("[TIME %03d] Process %-5s: %s\n", time, pid, status);
}

void log_gantt(const char* pid, int start, int end) {
    // This will eventually feed into your Gantt chart generator
    printf("Gantt: %s [%d - %d]\n", pid, start, end);
}