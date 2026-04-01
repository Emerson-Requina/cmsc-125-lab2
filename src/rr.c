#include <stdio.h>
#include <stdlib.h>
#include "process.h"
#include "scheduler.h"

#define MAX_PROCESSES 100

// The active queue being processed
static Process* ready_queue[MAX_PROCESSES];
static int head = 0, tail = 0;

// Temporary buffer for new arrivals that aren't allowed in yet
static Process* arrival_buffer[MAX_PROCESSES];
static int buffer_count = 0;

// Temporary buffer for processes that just finished a slice and are waiting for the queue to empty
static Process* finished_slice_buffer[MAX_PROCESSES];
static int finished_count = 0;

static int time_slice_counter = 0;

Process* pick_rr(Process *procs, int count, int time, Process *current, SchedulerConfig *config) {
    int q = config->quantum;

    // 1. Capture ALL new arrivals into a HOLDING BUFFER (not the ready queue)
    for (int i = 0; i < count; i++) {
        if (procs[i].arrival_time == time) {
            if (procs[i].remaining_time == procs[i].burst_time && procs[i].start_time == -1) {
                arrival_buffer[buffer_count++] = &procs[i];
            }
        }
    }

    // 2. Manage the current running process
    if (current != NULL) {
        time_slice_counter++;

        // If process finished its burst early
        if (current->remaining_time <= 0) {
            current = NULL;
            time_slice_counter = 0;
        } 
        // If process finished its time slice
        else if (time_slice_counter >= q) {
            // Move to "Finished Slice" buffer, NOT the ready queue yet
            finished_slice_buffer[finished_count++] = current;
            current = NULL;
            time_slice_counter = 0;
        } else {
            // Continue running current process until slice or burst ends
            return current;
        }
    }

    // 3. If no one is running, try to pick from the Ready Queue
    if (current == NULL) {
        if (head < tail) {
            current = ready_queue[head++];
            time_slice_counter = 0;
        } 
        // 4. If Ready Queue is EMPTY, perform the "Batch Update"
        else {
            // Reset queue pointers
            head = 0;
            tail = 0;

            // First: Re-enqueue processes that finished their slices (the "previous" generation)
            for (int i = 0; i < finished_count; i++) {
                ready_queue[tail++] = finished_slice_buffer[i];
            }
            finished_count = 0;

            // Second: Finally enqueue the new arrivals that were waiting
            for (int i = 0; i < buffer_count; i++) {
                ready_queue[tail++] = arrival_buffer[i];
            }
            buffer_count = 0;

            // Now try to pick from the newly populated queue
            if (head < tail) {
                current = ready_queue[head++];
                time_slice_counter = 0;
            }
        }
    }

    return current;
}

void reset_scheduler_state() {
    head = 0;
    tail = 0;
    buffer_count = 0;
    finished_count = 0;
    time_slice_counter = 0;
    for (int i = 0; i < MAX_PROCESSES; i++) {
        ready_queue[i] = NULL;
        arrival_buffer[i] = NULL;
        finished_slice_buffer[i] = NULL;
    }
}