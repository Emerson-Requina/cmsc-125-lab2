#include <stdio.h>
#include <stdlib.h>
#include "../include/process.h"

#define MAX_PROCESSES 100
static Process* ready_queue[MAX_PROCESSES];
static int head = 0, tail = 0;
static int time_slice_counter = 0;
static int quantum = 4;

void set_rr_quantum(int q) {
    quantum = q;
}

Process* pick_rr(Process *procs, int count, int time, Process *current) {
    // 1. Quantum Logic: Handle the CURRENT process first
    if (current != NULL) {
        time_slice_counter++;

        // If it used its quantum but still has work to do...
        if (time_slice_counter >= quantum && current->remaining_time > 0) {
            // Re-add to the end of the queue BEFORE checking new arrivals
            ready_queue[tail++] = current;
            time_slice_counter = 0;
            current = NULL; // Flag that we need a new process
        } 
        // If it actually finished its work, just reset the counter
        else if (current->remaining_time <= 0) {
            time_slice_counter = 0;
            current = NULL;
        }
    }

    // 2. New Arrivals: Add processes arriving EXACTLY at this 'time'
    for (int i = 0; i < count; i++) {
        if (procs[i].arrival_time == time) {
            // New arrivals go after the timed-out process
            ready_queue[tail++] = &procs[i];
        }
    }

    // 3. Selection: Return the next process in line
    if (current == NULL) {
        if (head < tail) {
            return ready_queue[head++];
        }
    }

    return current;
}