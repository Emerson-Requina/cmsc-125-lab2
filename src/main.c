#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/scheduler.h"
#include "../include/metrics.h"
#include "../include/gantt.h"
#include "../include/logger.h"
#include "../include/mlfq.h" // Added for MLFQ structures

// External declarations for the "Picker" functions
extern Process* pick_fcfs(Process *procs, int count, int time, Process *current);
extern Process* pick_sjf(Process *procs, int count, int time, Process *current);
extern Process* pick_stcf(Process *procs, int count, int time, Process *current);
extern Process* pick_rr(Process *procs, int count, int time, Process *current);
extern Process* pick_mlfq(Process *procs, int count, int time, Process *current); // Added MLFQ

extern void set_rr_quantum(int q);
extern void set_mlfq_config(MLFQConfig *cfg); // New setter for MLFQ module

int main(int argc, char *argv[]) {
    char *input_file = NULL;
    char *mlfq_file = "config/mlfq.conf"; // Default path
    char *algo_name = "FCFS"; 
    int quantum = 10;         

    // 1. Argument Parsing
    for (int i = 1; i < argc; i++) {
        if (strncmp(argv[i], "--input=", 8) == 0) {
            input_file = argv[i] + 8;
        } else if (strncmp(argv[i], "--algorithm=", 12) == 0) {
            algo_name = argv[i] + 12;
        } else if (strncmp(argv[i], "--quantum=", 10) == 0) {
            quantum = atoi(argv[i] + 10);
        } else if (strncmp(argv[i], "--mlfq-config=", 14) == 0) {
            mlfq_file = argv[i] + 14;
        }
    }

    if (!input_file) {
        fprintf(stderr, "Usage: %s --algorithm=<ALGO> --input=<FILE> [--quantum=<INT>] [--mlfq-config=<FILE>]\n", argv[0]);
        return 1;
    }

    int process_count = 0;
    Process *processes = load_processes(input_file, &process_count);

    if (!processes || process_count == 0) {
        fprintf(stderr, "Failed to load processes from %s\n", input_file);
        return 1;
    }

    AlgorithmPicker picker = NULL;
    int is_preemptive = 0;
    MLFQConfig *mlfq_cfg = NULL;

    // 2. Expanded Dispatcher Logic
    if (strcmp(algo_name, "FCFS") == 0) {
        picker = pick_fcfs;
        is_preemptive = 0;
    } else if (strcmp(algo_name, "SJF") == 0) {
        picker = pick_sjf;
        is_preemptive = 0;
    } else if (strcmp(algo_name, "STCF") == 0) {
        picker = pick_stcf;
        is_preemptive = 1;
    } else if (strcmp(algo_name, "RR") == 0) {
        set_rr_quantum(quantum);
        picker = pick_rr;
        is_preemptive = 1;
    } else if (strcmp(algo_name, "MLFQ") == 0) {
        // Load the MLFQ rules from the .conf file
        mlfq_cfg = load_mlfq_config(mlfq_file);
        if (!mlfq_cfg) {
            free(processes);
            return 1;
        }
        set_mlfq_config(mlfq_cfg); // Pass config to the mlfq.c module
        picker = pick_mlfq;
        is_preemptive = 1; // MLFQ must be preemptive to handle quantums/boosts
    } else {
        fprintf(stderr, "Algorithm %s not recognized.\n", algo_name);
        free(processes);
        return 1;
    }

    printf("Starting %s Simulation...\n", algo_name);
    
    // 3. The Engine
run_simulation(processes, process_count, picker, is_preemptive);

// 4. Post-Simulation Reporting
calculate_metrics(processes, process_count);
print_gantt_chart(1); // Micro-gantt call

// New: Analysis trigger
if (strcmp(algo_name, "MLFQ") == 0) {
    print_mlfq_behavior_report(processes, process_count, mlfq_cfg);
}

    // 5. Cleanup
    if (mlfq_cfg) free(mlfq_cfg);
    free(processes);
    
    return 0;
}