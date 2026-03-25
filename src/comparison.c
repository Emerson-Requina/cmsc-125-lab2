#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/comparison.h"
#include "../include/scheduler.h"
#include "../include/mlfq.h"

// External setters for RR and MLFQ
extern void set_rr_quantum(int q);
extern void set_mlfq_config(MLFQConfig *cfg);

// Picker function declarations
extern Process* pick_fcfs(Process *procs, int count, int time, Process *current);
extern Process* pick_sjf(Process *procs, int count, int time, Process *current);
extern Process* pick_stcf(Process *procs, int count, int time, Process *current);
extern Process* pick_rr(Process *procs, int count, int time, Process *current);
extern Process* pick_mlfq(Process *procs, int count, int time, Process *current);

void reset_procs(Process *dest, Process *src, int count) {
    for (int i = 0; i < count; i++) {
        dest[i] = src[i]; // Copy original data
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
    Process *temp_procs = malloc(sizeof(Process) * count);

    // Load MLFQ config once for the whole benchmark
    MLFQConfig *mlfq_cfg = load_mlfq_config(mlfq_conf);

    for (int i = 0; i < num_algos; i++) {
        reset_procs(temp_procs, original_procs, count);
        strcpy(results[i].algo_name, algos[i]);

        AlgorithmPicker picker = NULL;
        int is_preemptive = 0;

        // Setup specific parameters for each run
        if (strcmp(algos[i], "FCFS") == 0) {
            picker = pick_fcfs;
        } else if (strcmp(algos[i], "SJF") == 0) {
            picker = pick_sjf;
        } else if (strcmp(algos[i], "STCF") == 0) {
            picker = pick_stcf;
            is_preemptive = 1;
        } else if (strcmp(algos[i], "RR") == 0) {
            set_rr_quantum(10); // Standard benchmark quantum
            picker = pick_rr;
            is_preemptive = 1;
        } else if (strcmp(algos[i], "MLFQ") == 0) {
            set_mlfq_config(mlfq_cfg);
            picker = pick_mlfq;
            is_preemptive = 1;
        }

        // Run the simulation (Silently - no logs during benchmark)
        run_simulation(temp_procs, count, picker, is_preemptive);

        // Calculate Averages
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

    display_comparison_table(results, num_algos);

    if (mlfq_cfg) free(mlfq_cfg);
    free(temp_procs);
}