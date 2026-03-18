#include <stdio.h>
#include <stdlib.h>
#include "../include/process.h"

#define MAX_PROCESSES 100
static Process* ready_queue[MAX_PROCESSES];
static int head = 0, tail = 0;
static int time_slice_counter = 0;
static int quantum = 4; // This could also be a setting from main.c

void set_rr_quantum(int q) {
    quantum = q;
}

Process* pick_rr(Process *procs, int count, int time, Process *current) {
    // 1. Check for New Arrivals: Add anyone who just arrived at this 'time'
    for (int i = 0; i < count; i++) {
        if (procs[i].arrival_time == time) {
            ready_queue[tail++] = &procs[i];
        }
    }

    // 2. Quantum Logic: If a process is running, check its timer
    if (current != NULL) {
        time_slice_counter++;

        // If it finished its work, reset timer
        if (current->remaining_time <= 0) {
            time_slice_counter = 0;
        } 
        // If it used its quantum but still has work, move to back of queue
        else if (time_slice_counter >= quantum) {
            ready_queue[tail++] = current;
            time_slice_counter = 0;
            current = NULL; // Force a switch
        }
    }

    // 3. Return the next in line
    if (current == NULL || time_slice_counter == 0) {
        if (head < tail) {
            return ready_queue[head++];
        }
    }

    return current;
}