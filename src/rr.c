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
    
    // 1. Quantum/Preemption Logic FIRST
    // We check if the current process needs to be moved to the back of the queue
    // BEFORE we add new arrivals, ensuring new arrivals are truly "last".
    if (current != NULL) {
        time_slice_counter++;

        // If it used its quantum but still has work to do...
        if (time_slice_counter >= q) {
            if (current->remaining_time > 0) {
                ready_queue[tail % MAX_PROCESSES] = current;
                tail++;
            }
            time_slice_counter = 0;
            current = NULL; // Force a reschedule
        } 
        // If it actually finished its work mid-quantum
        else if (current->remaining_time <= 0) {
            time_slice_counter = 0;
            current = NULL;
        }
    }

    // 2. Handle New Arrivals SECOND
    // By placing this after the preemption logic, these processes 
    // are guaranteed to be behind a preempted process in the circular buffer.
    for (int i = 0; i < count; i++) {
        if (procs[i].arrival_time == time) {
            // Safety check: ensure we don't double-add if pick_rr is called 
            // multiple times at the same 'time' (depends on your simulation loop)
            if (procs[i].remaining_time == procs[i].burst_time && procs[i].start_time == -1) {
                ready_queue[tail % MAX_PROCESSES] = &procs[i];
                tail++;
            }
        }
    }

    // 3. Selection
    if (current == NULL) {
        if (head < tail) {
            current = ready_queue[head % MAX_PROCESSES];
            head++;
            time_slice_counter = 0; 
        }
    }

    return current;
}