#include <stdio.h>
#include "../include/scheduler.h"
#include "../include/logger.h"

void run_simulation(Process *procs, int count, AlgorithmPicker picker, int is_preemptive) {
    int current_time = 0;
    int completed = 0;
    Process *current_running = NULL;

    while (completed < count) {
        // 1. If we are preemptive OR the CPU is idle, ask the "Brain" who is next
        if (is_preemptive || current_running == NULL) {
            current_running = picker(procs, count, current_time, current_running);
        }

        if (current_running != NULL) {
            // Check for Response Time (First time running)
            if (!current_running->responded) {
                current_running->start_time = current_time;
                current_running->responded = 1;
            }

            log_step(current_time, current_running->pid, "Running");

            // Execute 1 unit of time
            current_running->remaining_time--;
            current_time++;

            if (current_running->remaining_time == 0) {
                current_running->finish_time = current_time;
                log_step(current_time, current_running->pid, "Finished");
                current_running = NULL; 
                completed++;
            }
        } else {
            // CPU is idle
            log_step(current_time, "IDLE", "-");
            current_time++;
        }
    }
}