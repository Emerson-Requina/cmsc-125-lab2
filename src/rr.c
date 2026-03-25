#include <stdio.h>
#include <stdlib.h>
#include "process.h"
#include "scheduler.h"

#define MAX_PROCESSES 100
static Process* ready_queue[MAX_PROCESSES];
static int head = 0, tail = 0;
static int time_slice_counter = 0;

Process* pick_rr(Process *procs, int count, int time, Process *current, SchedulerConfig *config) {
    int q = config->quantum;
    
    // 1. Handle New Arrivals First (Standard RR Tie-breaking)
    for (int i = 0; i < count; i++) {
        // Only add if it just arrived AND hasn't been queued before
        // We use 'ready_state' or similar if you added it to the struct, 
        // otherwise check arrival_time == time
        if (procs[i].arrival_time == time) {
            ready_queue[tail % MAX_PROCESSES] = &procs[i];
            tail++;
        }
    }

    // 2. Quantum/Preemption Logic
    if (current != NULL) {
        time_slice_counter++;

        // If it used its quantum but still has work to do...
        if (time_slice_counter >= q && current->remaining_time > 0) {
            // Re-add to the end of the queue
            ready_queue[tail % MAX_PROCESSES] = current;
            tail++;
            time_slice_counter = 0;
            current = NULL; // Force a reschedule
        } 
        // If it actually finished its work
        else if (current->remaining_time <= 0) {
            time_slice_counter = 0;
            current = NULL;
        }
    }

    // 3. Selection
    if (current == NULL) {
        if (head < tail) {
            current = ready_queue[head % MAX_PROCESSES];
            head++;
            time_slice_counter = 0; // Reset counter for the new process
        }
    }

    return current;
}