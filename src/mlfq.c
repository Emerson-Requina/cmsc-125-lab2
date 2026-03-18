#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/scheduler.h"

void run_mlfq(Process *processes, int count, MLFQConfig config) {
    int current_time = 0;
    int completed = 0;
    
    int current_priority[count];
    int allotment_left[count];
    int finished[count];

    for (int i = 0; i < count; i++) {
        current_priority[i] = 0;
        allotment_left[i] = config.queues[0].allotment;
        finished[i] = 0;
        processes[i].responded = 0;
        // Initialize demotion times to 0
        for(int j = 0; j < 10; j++) processes[i].demotion_times[j] = 0;
    }

    while (completed < count) {
        // 1. Priority Boost
        if (current_time > 0 && current_time % config.boost_period == 0) {
            printf("t=%d:   Priority boost: all processes -> Q0\n", current_time);
            for (int i = 0; i < count; i++) {
                if (!finished[i]) {
                    current_priority[i] = 0;
                    allotment_left[i] = config.queues[0].allotment;
                }
            }
        }

        // 2. Select Process
        int idx = -1;
        for (int q = 0; q < config.num_queues; q++) {
            for (int i = 0; i < count; i++) {
                if (!finished[i] && processes[i].arrival_time <= current_time && current_priority[i] == q) {
                    idx = i;
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

            int q_val = config.queues[current_priority[idx]].quantum;
            int max_run = (q_val == -1) ? processes[idx].remaining_time : q_val;

            for (int i = 0; i < max_run; i++) {
                // Check arrival at start of unit
                for (int j = 0; j < count; j++) {
                    if (processes[j].arrival_time == current_time && current_time != 0) {
                         // Note: t=0 handled in main or initial loop usually
                         printf("t=%d:   Process %s enters Q0\n", current_time, processes[j].pid);
                    }
                }

                current_time++;
                processes[idx].remaining_time--;

                // 3. Demotion Logic
                if (q_val != -1) {
                    allotment_left[idx]--;
                    if (allotment_left[idx] <= 0 && current_priority[idx] < config.num_queues - 1) {
                        current_priority[idx]++;
                        
                        // RECORD DEMOTION TIME
                        processes[idx].demotion_times[current_priority[idx]] = current_time;
                        
                        allotment_left[idx] = config.queues[current_priority[idx]].allotment;
                        printf("t=%d:   Process %s -> Q%d (exhausted Q%d allotment)\n", 
                               current_time, processes[idx].pid, current_priority[idx], current_priority[idx]-1);
                        break; 
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
                if (current_time % config.boost_period == 0) break;
            }
        } else {
            current_time++;
        }
    }
}