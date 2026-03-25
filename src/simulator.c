#include <stdio.h>
#include <string.h>
#include "scheduler.h"
#include "logger.h"
#include "gantt.h"

void run_simulation(Process *procs, int count, AlgorithmPicker picker, int is_preemptive, SchedulerConfig *config) {
    int current_time = 0;
    int completed = 0;
    Process *current_running = NULL;

    // Gantt tracking variables
    int burst_start_time = 0;

    while (completed < count) {
        // 1. Determine if we need to pick a new process
        Process *next_up = current_running;
        
        // If preemptive, we check every tick. If not, we only check if the CPU is empty.
        if (is_preemptive || current_running == NULL) {
            next_up = picker(procs, count, current_time, current_running, config);
        }

        // 2. CONTEXT SWITCH DETECTION: Did the process change?
        if (next_up != current_running) {
            // Record the block that just finished in the Gantt chart
            if (current_running != NULL) {
                log_execution(current_running->pid, burst_start_time, current_time);
            } else if (current_time > 0 && burst_start_time < current_time) {
                // This records gaps where the CPU was IDLE
                log_execution("IDLE", burst_start_time, current_time);
            }
            
            burst_start_time = current_time;
            current_running = next_up;
        }

        // 3. Execution Logic
        if (current_running != NULL) {
            // Handle Response Time (first time running)
            if (!current_running->responded) {
                current_running->start_time = current_time;
                current_running->responded = 1;
            }

            log_step(current_time, current_running->pid, "Running");

            current_running->remaining_time--;
            current_time++;

            // 4. Completion Check
            if (current_running->remaining_time == 0) {
                // Save the final burst for this process before it disappears
                log_execution(current_running->pid, burst_start_time, current_time);
                
                current_running->finish_time = current_time;
                log_step(current_time, current_running->pid, "Finished");
                
                completed++;
                current_running = NULL; 
                burst_start_time = current_time; // Reset for the next process/idle period
            }
        } else {
            // CPU is idle
            log_step(current_time, "IDLE", "-");
            current_time++;
        }
    }
}