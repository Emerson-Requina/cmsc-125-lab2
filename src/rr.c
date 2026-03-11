#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/scheduler.h"
#include "../include/gantt.h"

void run_rr(Process *processes, int count, int quantum) {
    int current_time = 0;
    int completed = 0;
    int *ready_queue = malloc(sizeof(int) * count);
    int head = 0, tail = 0;
    int in_queue[count]; // Track if process is already in the ready queue

    for (int i = 0; i < count; i++) in_queue[i] = 0;

    printf("Starting Round Robin Simulation (Quantum: %d)...\n", quantum);

    // Continue until all processes are finished
    while (completed < count) {
        // 1. Add arriving processes to the ready queue
        for (int i = 0; i < count; i++) {
            if (processes[i].arrival_time <= current_time && processes[i].remaining_time > 0 && !in_queue[i]) {
                ready_queue[tail++] = i;
                in_queue[i] = 1;
            }
        }

        if (head == tail) { // Queue is empty, CPU is idle
            current_time++;
            continue;
        }

        // 2. Pick the next process from the head
        int idx = ready_queue[head++];
        Process *p = &processes[idx];

        // Mark start time/response time if it's the first execution
        if (!p->responded) {
            p->start_time = current_time;
            p->responded = 1;
        }

        // 3. Determine execution time (min of remaining time or quantum)
        int run_time = (p->remaining_time < quantum) ? p->remaining_time : quantum;
        
        log_execution(p->pid, current_time, current_time + run_time);
        
        p->remaining_time -= run_time;
        int prev_time = current_time;
        current_time += run_time;

        // 4. Check for new arrivals DURING the time slice 
        // (Important: newcomers get priority over the demoted process in standard RR)
        for (int i = 0; i < count; i++) {
            if (processes[i].arrival_time > prev_time && processes[i].arrival_time <= current_time && !in_queue[i]) {
                ready_queue[tail++] = i;
                in_queue[i] = 1;
            }
        }

        // 5. Handle process completion or re-queueing
        if (p->remaining_time == 0) {
            p->finish_time = current_time;
            completed++;
        } else {
            // Still has work? Put it back at the end of the queue
            ready_queue[tail++] = idx;
        }
    }

    free(ready_queue);
    printf("Round Robin Simulation Complete.\n");
}