#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/comparison.h"
#include "../include/scheduler.h"
#include "../include/mlfq.h"

/**
 * reset_procs - Restores process states for a fresh simulation run.
 * We include any specific fields like demotion_time or priority to ensure
 * MLFQ and RR runs don't inherit data from previous algorithm runs.
 */
void reset_procs(Process *dest, Process *src, int count) {
    for (int i = 0; i < count; i++) {
        dest[i] = src[i]; 
        dest[i].remaining_time = src[i].burst_time;
        dest[i].responded = 0;
        dest[i].start_time = -1;
        dest[i].finish_time = 0;
        dest[i].priority = 0;
        dest[i].time_in_queue = 0;
        dest[i].lowest_priority_attained = 0;
        dest[i].demotion_time = -1;
        // If you added 'enqueued' or similar for your RR fix, reset it here:
        // dest[i].enqueued = 0; 
    }
}

void run_comparative_analysis(Process *original_procs, int count, const char *mlfq_conf) {
    char *algos[] = {"FCFS", "SJF", "STCF", "RR", "MLFQ"};
    int num_algos = 5;
    SchedulingMetrics results[5];
    Process *temp_procs = malloc(sizeof(Process) * count);

    if (!temp_procs) {
        fprintf(stderr, "Memory allocation failed for comparative analysis.\n");
        return;
    }

    // Initialize the configuration object once
    SchedulerConfig config;
    config.quantum = 10; // Standard quantum for benchmark
    config.mlfq_cfg = load_mlfq_config(mlfq_conf);

    for (int i = 0; i < num_algos; i++) {
        reset_procs(temp_procs, original_procs, count);
        strncpy(results[i].algo_name, algos[i], sizeof(results[i].algo_name) - 1);

        AlgorithmPicker picker = NULL;
        int is_preemptive = 0;

        // Select the appropriate picker function
        if (strcmp(algos[i], "FCFS") == 0) {
            picker = pick_fcfs;
        } else if (strcmp(algos[i], "SJF") == 0) {
            picker = pick_sjf;
        } else if (strcmp(algos[i], "STCF") == 0) {
            picker = pick_stcf;
            is_preemptive = 1;
        } else if (strcmp(algos[i], "RR") == 0) {
            picker = pick_rr;
            is_preemptive = 1;
        } else if (strcmp(algos[i], "MLFQ") == 0) {
            picker = pick_mlfq;
            is_preemptive = 1;
        }

        // Execute the simulation. 
        // Note: No more 'set_rr_quantum' or 'set_mlfq_config' calls.
        // The picker functions now extract these from the 'config' pointer.
        run_simulation(temp_procs, count, picker, is_preemptive, &config);

        // Metric Calculation
        double tt = 0, wt = 0, rt = 0;
        for (int j = 0; j < count; j++) {
            int turn = temp_procs[j].finish_time - temp_procs[j].arrival_time;
            tt += turn;
            wt += (turn - temp_procs[j].burst_time);
            rt += (temp_procs[j].start_time - temp_procs[j].arrival_time);
        }
        
        results[i].avg_turnaround_time = tt / count;
        results[i].avg_waiting_time = wt / count;
        results[i].avg_response_time = rt / count;
    }

    // Output the final results table
    display_comparison_table(results, num_algos);

    // Cleanup
    if (config.mlfq_cfg) free(config.mlfq_cfg);
    free(temp_procs);
}