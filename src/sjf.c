#include <stdio.h>
#include <limits.h>
#include "scheduler.h"

Process* pick_sjf(Process *procs, int count, int time, Process *current) {
    // 1. Non-preemptive Check: If someone is already on the CPU, 
    // they stay until they are done (remaining_time == 0).
    if (current != NULL && current->remaining_time > 0) {
        return current;
    }

    // 2. Triage: Look for the arrived process with the shortest TOTAL burst time.
    Process *best = NULL;
    for (int i = 0; i < count; i++) {
        // Must have arrived and still have work to do
        if (procs[i].arrival_time <= time && procs[i].remaining_time > 0) {
            if (best == NULL || procs[i].burst_time < best->burst_time) {
                best = &procs[i];
            }
        }
    }

    return best; // Returns NULL if no one has arrived yet
}