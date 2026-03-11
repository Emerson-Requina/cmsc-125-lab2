#include <stdio.h>
#include "../include/scheduler.h"
#include "../include/gantt.h" // 1. Added this include

void run_fcfs(Process *processes, int count) {
    int current_time = 0;

    for (int i = 0; i < count; i++) {
        // If the CPU is idle waiting for the next process to arrive
        if (current_time < processes[i].arrival_time) {
            current_time = processes[i].arrival_time;
        }

        // Record Response Time
        processes[i].start_time = current_time;
        
        // 2. Log the execution BEFORE updating current_time
        log_execution(processes[i].pid, current_time, current_time + processes[i].burst_time);

        // Execute to completion
        current_time += processes[i].burst_time;
        processes[i].finish_time = current_time;
        
        // Calculate standard metrics
        processes[i].waiting_time = processes[i].start_time - processes[i].arrival_time;
        
        printf("Time %d: %s finished\n", current_time, processes[i].pid);
    }
}