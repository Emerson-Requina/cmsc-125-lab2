#include <stdio.h>
#include <limits.h>
#include "../include/scheduler.h"
#include "../include/gantt.h"

void run_stcf(Process *processes, int count) {
    int current_time = 0;
    int completed = 0;
    int last_idx = -1;      // Track the previous process to detect switches
    int burst_start = 0;    // Track when the current process started its continuous run

    printf("Starting STCF Simulation...\n");

    while (completed < count) {
        int idx = -1;
        int min_remaining = INT_MAX;

        // 1. Find the process with the shortest remaining time
        for (int i = 0; i < count; i++) {
            if (processes[i].arrival_time <= current_time && processes[i].remaining_time > 0) {
                if (processes[i].remaining_time < min_remaining) {
                    min_remaining = processes[i].remaining_time;
                    idx = i;
                }
            }
        }

        // 2. Handle Context Switching for the Gantt Chart
        if (idx != last_idx) {
            // If a different process was running, log its completed burst
            if (last_idx != -1) {
                log_execution(processes[last_idx].pid, burst_start, current_time);
            }
            // Update tracking for the new process
            burst_start = current_time;
            last_idx = idx;
        }

        if (idx != -1) {
            // Record initial response time
            if (!processes[idx].responded) {
                processes[idx].start_time = current_time;
                processes[idx].responded = 1;
            }

            // Execute for 1 time unit
            processes[idx].remaining_time--;
            current_time++;

            // 3. Handle Process Completion
            if (processes[idx].remaining_time == 0) {
                processes[idx].finish_time = current_time;
                completed++;
                
                // Log the final burst of this process
                log_execution(processes[idx].pid, burst_start, current_time);
                
                printf("Time %d: %s (STCF) finished\n", current_time, processes[idx].pid);
                
                // Reset tracking since this process is done
                last_idx = -1; 
            }
        } else {
            current_time++; // CPU is idle
            last_idx = -1;
        }
    }
}