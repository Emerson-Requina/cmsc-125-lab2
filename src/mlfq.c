#include <stdio.h>
#include <stdlib.h>
#include "mlfq.h"
#include "scheduler.h"

static int time_slice_counter = 0;

Process* pick_mlfq(Process *procs, int count, int time, Process *current, SchedulerConfig *config) {
    MLFQConfig *mlfq_config = config->mlfq_cfg; // Pulls config from the parameter and not a global setter
    if (!mlfq_config) return NULL;

    // 1. Priority Boost (Existing logic is fine)
    if (time > 0 && time % mlfq_config->boost_period == 0) {
        printf("t=%d:  Priority boost: all processes -> Q0\n", time);
        for (int i = 0; i < count; i++) {
            if (procs[i].remaining_time > 0) {
                procs[i].priority = 0;
                procs[i].time_in_queue = 0; // Reset individual tracking
            }
        }
        time_slice_counter = 0;
        current = NULL;
    }

    if (current != NULL) {
        time_slice_counter++;
        current->time_in_queue++; // USE THE STRUCT FIELD HERE

        int q_idx = current->priority;
        int q_quantum = mlfq_config->levels[q_idx].quantum;
        int q_allotment = mlfq_config->levels[q_idx].allotment;

        if (current->remaining_time <= 0) {
            time_slice_counter = 0;
            current = NULL;
        } 
        // 2. Demotion Logic
        else if (q_allotment != -1 && current->time_in_queue >= q_allotment) {
            if (current->priority < mlfq_config->level_count - 1) {
                current->priority++;
                
                // LOG THE DEMOTION IMMEDIATELY
                printf("t=%d:  Process %s -> Q%d (exhausted Q%d allotment)\n", 
                       time, current->pid, current->priority, current->priority - 1);

                if (current->demotion_time == -1) {
                    current->demotion_time = time;
                }
                current->lowest_priority_attained = current->priority;
            }
            current->time_in_queue = 0; // Reset for the next queue level
            time_slice_counter = 0;
            current = NULL; // Force reschedule
        }
        else if (q_quantum != -1 && time_slice_counter >= q_quantum) {
            time_slice_counter = 0;
            // Note: We DO NOT reset current->time_in_queue here!
            // It stays accumulated until they hit the allotment.
            current = NULL; 
        }
    }

    // 3. Selection Logic
    if (current == NULL) {
        for (int q = 0; q < mlfq_config->level_count; q++) {
            for (int i = 0; i < count; i++) {
                if (procs[i].arrival_time <= time && procs[i].remaining_time > 0 && procs[i].priority == q) {
                    time_slice_counter = 0;
                    return &procs[i];
                }
            }
        }
    }
    return current;
}