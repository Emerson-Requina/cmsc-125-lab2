#include <stdio.h>
#include <limits.h>
#include "../include/scheduler.h"

void run_sjf(Process *processes, int count) {
    int current_time = 0;
    int completed = 0;
    int is_completed[count]; 
    for(int i=0; i<count; i++) is_completed[i] = 0;

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
            current_time += processes[idx].burst_time;
            processes[idx].finish_time = current_time;
            processes[idx].waiting_time = processes[idx].start_time - processes[idx].arrival_time;
            
            is_completed[idx] = 1;
            completed++;
            printf("Time %d: %s (SJF) finished\n", current_time, processes[idx].pid);
        } else {
            current_time++; // CPU is idle
        }
    }
}