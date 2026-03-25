#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/comparison.h"
#include "../include/scheduler.h"
#include "../include/mlfq.h"

/**
 * reset_procs - Restores process states for a fresh simulation run.
 * This ensures that a process modified by FCFS doesn't carry 
 * 'finish_time' or 'priority' data into the RR or MLFQ runs.
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
    }
}

void run_comparative_analysis(Process *original_procs, int count, const char *mlfq_conf) {
    char *algos[] = {"FCFS", "SJF", "STCF", "RR", "MLFQ"};
    int num_algos = 5;
    SchedulingMetrics results[5];
    
    // 1. Ownership: We allocate a temporary work array for the benchmark
    Process *temp_procs = malloc(sizeof(Process) * count);
    if (!temp_procs) {
        fprintf(stderr, "Critical: Memory allocation failed for comparison.\n");
        return;
    }

    // 2. Load MLFQ config once for the entire comparative session
    MLFQConfig *mlfq_cfg = load_mlfq_config(mlfq_conf);
    
    // Prepare the configuration wrapper
    SchedulerConfig config;
    config.quantum = 10; 
    config.mlfq_cfg = mlfq_cfg;

    for (int i = 0; i < num_algos; i++) {
        // 3. Reset state: Every algorithm starts with a "clean" copy of the data
        reset_procs(temp_procs, original_procs, count);
        strncpy(results[i].algo_name, algos[i], sizeof(results[i].algo_name) - 1);

        AlgorithmPicker picker = NULL;
        int is_preemptive = 0;

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

        // 4. Run Simulation: We pass the config pointer (maintaining clear ownership)
        run_simulation(temp_procs, count, picker, is_preemptive, &config);

        // 5. Calculate Metrics for this specific run
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

    // Output the results table
    display_comparison_table(results, num_algos);

    // 6. Final Cleanup: Explicitly free nested resources
    if (mlfq_cfg) {
        free_mlfq_config(mlfq_cfg);
    }
    
    if (temp_procs) {
        free_processes(temp_procs);
    }
}