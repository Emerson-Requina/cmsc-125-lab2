#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/scheduler.h"
#include "../include/gantt.h"

void run_mlfq(Process *processes, int count, MLFQConfig config) {
    int current_time = 0;
    int completed = 0;
    // int last_boost_time = 0;
    
    int current_priority[count];
    int allotment_left[count];
    int finished[count];

    for (int i = 0; i < count; i++) {
        current_priority[i] = 0;
        allotment_left[i] = config.queues[0].allotment;
        finished[i] = 0;
    }

    // Initial arrival check for t=0
    for (int i = 0; i < count; i++) {
        if (processes[i].arrival_time == 0) {
            printf("t=0:   Process %s enters Q0\n", processes[i].pid);
        }
    }

    while (completed < count) {
        // 1. Priority Boost Check
        if (current_time > 0 && current_time % config.boost_period == 0) {
            printf("t=%d:   Priority boost: all processes -> Q0\n", current_time);
            for (int i = 0; i < count; i++) {
                if (!finished[i]) {
                    current_priority[i] = 0;
                    allotment_left[i] = config.queues[0].allotment;
                }
            }
        }

        // 2. Select highest priority process available
        int idx = -1;
        int target_q = -1;
        for (int q = 0; q < config.num_queues; q++) {
            for (int i = 0; i < count; i++) {
                if (!finished[i] && processes[i].arrival_time <= current_time && current_priority[i] == q) {
                    idx = i;
                    target_q = q;
                    break;
                }
            }
            if (idx != -1) break;
        }

        if (idx != -1) {
            if (!processes[idx].responded) {
                processes[idx].start_time = current_time;
                processes[idx].responded = 1;
            }

            int q_val = config.queues[target_q].quantum;
            // If Q is FCFS (-1), run until completion or next arrival/boost
            int max_run = (q_val == -1) ? processes[idx].remaining_time : q_val;

            // Execute the process step-by-step for accurate tracing
            for (int i = 0; i < max_run; i++) {
                current_time++;
                processes[idx].remaining_time--;
                
                // Track NEW arrivals during this execution step
                for (int j = 0; j < count; j++) {
                    if (processes[j].arrival_time == current_time) {
                        printf("t=%d:   Process %s enters Q0\n", current_time, processes[j].pid);
                    }
                }

                // Check for demotion (if not the bottom queue)
                if (q_val != -1) {
                    allotment_left[idx]--;
                    if (allotment_left[idx] <= 0 && current_priority[idx] < config.num_queues - 1) {
                        current_priority[idx]++;
                        allotment_left[idx] = config.queues[current_priority[idx]].allotment;
                        printf("t=%d:   Process %s -> Q%d (exhausted Q%d allotment)\n", 
                                current_time, processes[idx].pid, current_priority[idx], current_priority[idx] - 1);
                        break; // Stop current burst to re-schedule
                    }
                }

                if (processes[idx].remaining_time == 0) {
                    processes[idx].finish_time = current_time;
                    processes[idx].priority = current_priority[idx];
                    finished[idx] = 1;
                    completed++;
                    printf("t=%d:   Process %s completes in Q%d\n", current_time, processes[idx].pid, current_priority[idx]);
                    break;
                }
            }
        } else {
            current_time++;
        }
    }
}