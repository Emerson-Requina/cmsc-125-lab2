#include <stdio.h>
#include <limits.h>
#include "scheduler.h"

Process* pick_stcf(Process *procs, int count, int time, Process *current, SchedulerConfig *config) {
    // Silence the 'unused parameter' error
    (void)current;
    (void)config;

    Process *best = NULL;
    for (int i = 0; i < count; i++) {
        if (procs[i].arrival_time <= time && procs[i].remaining_time > 0) {
            if (best == NULL || procs[i].remaining_time < best->remaining_time) {
                best = &procs[i];
            }
        }
    }
    return best;
}