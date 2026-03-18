#include <stdio.h>
#include "../include/scheduler.h"

Process* pick_fcfs(Process *procs, int count, int time, Process *current) {
    // Silence the 'unused parameter' error
    (void)current;
    
    Process *best = NULL;
    for (int i = 0; i < count; i++) {
        if (procs[i].arrival_time <= time && procs[i].remaining_time > 0) {
            if (best == NULL || procs[i].arrival_time < best->arrival_time) {
                best = &procs[i];
            }
        }
    }
    return best;
}