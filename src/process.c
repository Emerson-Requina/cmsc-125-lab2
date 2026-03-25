#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "process.h"

Process* load_processes(const char *filename, int *count) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "\033[0;31m[ERROR]\033[0m Could not open workload file: %s\n", filename);
        return NULL;
    }

    char line[256];
    int capacity = 10;
    int line_num = 0;
    *count = 0;
    
    Process *processes = malloc(sizeof(Process) * capacity);
    if (!processes) {
        fclose(file);
        return NULL;
    }

    while (fgets(line, sizeof(line), file)) {
        line_num++;
        // Skip comments and empty lines
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r' || line[0] == ' ') continue;

        Process p;
        // Format: PID ArrivalTime BurstTime
        int matched = sscanf(line, "%s %d %d", p.pid, &p.arrival_time, &p.burst_time);
        
        if (matched == 3) {
            // Basic logical validation (No negative times)
            if (p.arrival_time < 0 || p.burst_time <= 0) {
                fprintf(stderr, "\033[0;31m[FATAL]\033[0m Invalid values at line %d: %s", line_num, line);
                goto fatal_error;
            }

            // Initialize simulation fields
            p.remaining_time = p.burst_time;
            p.start_time = -1;
            p.finish_time = 0;
            p.waiting_time = 0;
            p.responded = 0;
            
            // Initialize MLFQ Specifics
            p.priority = 0;
            p.time_in_queue = 0;
            p.lowest_priority_attained = 0;
            p.demotion_time = -1;

            if (*count >= capacity) {
                capacity *= 2;
                Process *temp = realloc(processes, sizeof(Process) * capacity);
                if (!temp) goto fatal_error;
                processes = temp;
            }
            processes[(*count)++] = p;
        } 
        else {
            // This catches "dfadsf" or missing columns
            fprintf(stderr, "\033[0;31m[FATAL]\033[0m Malformed process data at line %d: %s", line_num, line);
            goto fatal_error;
        }
    }

    fclose(file);
    
    if (*count == 0) {
        fprintf(stderr, "\033[0;33m[WARNING]\033[0m Workload file was empty.\n");
        free(processes);
        return NULL;
    }

    return processes;

fatal_error:
    fclose(file);
    if (processes) free(processes);
    *count = 0;
    return NULL;
}

void free_processes(Process *procs) {
    if (!procs) return;

    free(procs);
}