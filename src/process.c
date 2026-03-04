#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/process.h"

Process* load_processes(const char *filename, int *count) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening workload file");
        return NULL;
    }

    char line[256];
    int capacity = 10;
    *count = 0;
    Process *processes = malloc(sizeof(Process) * capacity);

    while (fgets(line, sizeof(line), file)) {
        // Skip comments and empty lines
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') continue;

        Process p;
        // Format: PID ArrivalTime BurstTime
        if (sscanf(line, "%s %d %d", p.pid, &p.arrival_time, &p.burst_time) == 3) {
            // Initialize simulation fields
            p.remaining_time = p.burst_time;
            p.start_time = -1;
            p.finish_time = 0;
            p.waiting_time = 0;
            p.responded = 0;
            p.priority = 0;
            p.time_in_queue = 0;

            if (*count >= capacity) {
                capacity *= 2;
                processes = realloc(processes, sizeof(Process) * capacity);
            }
            processes[(*count)++] = p;
        }
    }

    fclose(file);
    return processes;
}