#include <stdio.h>
#include <string.h>
#include "scheduler.h"
#include "logger.h"
#include "gantt.h"

void run_simulation(Process *procs, int count, AlgorithmPicker picker, int is_preemptive, SchedulerConfig *config) {
    int current_time = 0;
    int completed = 0;
    Process *current_running = NULL;
    int burst_start_time = 0;

    while (completed < count) {
        // 1. Log arrivals for the current tick
        for (int i = 0; i < count; i++) {
            if (procs[i].arrival_time == current_time) {
                log_arrival(current_time, procs[i].pid);
            }
        }

        // 2. Pick the process for THIS tick
        Process *next_up = current_running;
        if (is_preemptive || current_running == NULL) {
            next_up = picker(procs, count, current_time, current_running, config);
        }

        // 3. Handle Context Switching
        if (next_up != current_running) {
            if (current_running != NULL) {
                if (current_running->remaining_time > 0) {
                    log_preemption(current_time, current_running->pid, next_up ? next_up->pid : "IDLE");
                }
                log_execution(current_running->pid, burst_start_time, current_time);
            } else if (burst_start_time < current_time) {
                log_execution("IDLE", burst_start_time, current_time);
            }
            
            current_running = next_up;
            burst_start_time = current_time;
        }

        // 4. Execute the process
        if (current_running != NULL) {
            if (!current_running->responded) {
                current_running->start_time = current_time;
                current_running->responded = 1;
            }

            log_step(current_time, current_running->pid, "Running");
            current_running->remaining_time--;

            // 5. Check Completion IMMEDIATELY
            if (current_running->remaining_time == 0) {
                completed++;
                current_running->finish_time = current_time + 1; // It finished during this tick
                log_execution(current_running->pid, burst_start_time, current_time + 1);
                log_step(current_time + 1, current_running->pid, "Finished");
                
                current_running = NULL; 
                burst_start_time = current_time + 1;
            }
        } else {
            log_step(current_time, "IDLE", "-");
        }

        // 6. GLOBAL TIME INCREMENT (At the very end)
        current_time++;
        if (current_time > 9999) {
    printf("DEBUG: MLFQ stuck. Completed: %d/%d, Current: %s\n", 
            completed, count, current_running ? current_running->pid : "NONE");
}
        // Safety break to prevent actual infinite loops while debugging
        if (current_time > 10000) { 
            fprintf(stderr, "Simulation timed out (Infinite IDLE loop).\n");
            break; 
        }
    }
}