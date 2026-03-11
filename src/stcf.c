#include <stdio.h>
#include <limits.h>
#include "../include/scheduler.h"

void run_stcf(Process *processes, int count) {
    int current_time = 0;
    int completed = 0;

    while (completed < count) {
        int idx = -1;
        int min_remaining = INT_MAX;

        // Find process with shortest remaining time among those that have arrived
        for (int i = 0; i < count; i++) {
            if (processes[i].arrival_time <= current_time && processes[i].remaining_time > 0) {
                if (processes[i].remaining_time < min_remaining) {
                    min_remaining = processes[i].remaining_time;
                    idx = i;
                }
            }
        }

        if (idx != -1) {
            // If this is the first time the process runs, set start_time
            if (!processes[idx].responded) {
                processes[idx].start_time = current_time;
                processes[idx].responded = 1;
            }

            // Execute for 1 time unit
            processes[idx].remaining_time--;
            current_time++;

            if (processes[idx].remaining_time == 0) {
                processes[idx].finish_time = current_time;
                completed++;
                printf("Time %d: %s (STCF) finished\n", current_time, processes[idx].pid);
            }
        } else {
            current_time++; // Idle
        }
    }
}