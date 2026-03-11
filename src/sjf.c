#include <stdio.h>
#include <limits.h>
#include "../include/scheduler.h"
#include "../include/gantt.h" // 1. Include the Gantt header

void run_sjf(Process *processes, int count) {
    int current_time = 0;
    int completed = 0;
    int is_completed[count]; 
    for(int i=0; i<count; i++) is_completed[i] = 0;

    printf("Starting SJF Simulation...\n");

    while (completed < count) {
        int idx = -1;
        int min_burst = INT_MAX;

        // Find the arrived process with the shortest burst
        for (int i = 0; i < count; i++) {
            if (processes[i].arrival_time <= current_time && !is_completed[i]) {
                if (processes[i].burst_time < min_burst) {
                    min_burst = processes[i].burst_time;
                    idx = i;
                }
            }
        }

        if (idx != -1) {
            processes[idx].start_time = current_time;

            // 2. Log the execution for the Gantt Chart
            log_execution(processes[idx].pid, current_time, current_time + processes[idx].burst_time);

            current_time += processes[idx].burst_time;
            processes[idx].finish_time = current_time;
            
            // SJF response time is the same as waiting time since it's non-preemptive
            processes[idx].responded = 1; 
            
            is_completed[idx] = 1;
            completed++;
            printf("Time %d: %s (SJF) finished\n", current_time, processes[idx].pid);
        } else {
            current_time++; // CPU is idle
        }
    }
}