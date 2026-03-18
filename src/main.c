#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/scheduler.h"
#include "../include/metrics.h"
#include "../include/gantt.h"
#include "../include/logger.h"

// External declarations for the "Picker" functions which only decide who runs next.
extern Process* pick_fcfs(Process *procs, int count, int time, Process *current);
extern Process* pick_sjf(Process *procs, int count, int time, Process *current);
extern Process* pick_stcf(Process *procs, int count, int time, Process *current);
extern Process* pick_rr(Process *procs, int count, int time, Process *current);
extern void set_rr_quantum(int q); // New setter to preserve --quantum flag

int main(int argc, char *argv[]) {
    char *input_file = NULL;
    char *algo_name = "FCFS"; // Default 
    int quantum = 10;         // Default quantum for RR

    // 1. Argument Parsing
    for (int i = 1; i < argc; i++) {
        if (strncmp(argv[i], "--input=", 8) == 0) {
            input_file = argv[i] + 8;
        } else if (strncmp(argv[i], "--algorithm=", 12) == 0) {
            algo_name = argv[i] + 12;
        } else if (strncmp(argv[i], "--quantum=", 10) == 0) {
            quantum = atoi(argv[i] + 10);
        }
    }

    if (!input_file) {
        fprintf(stderr, "Usage: %s --algorithm=<ALGO> --input=<FILE> [--quantum=<INT>]\n", argv[0]);
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

    // 2. Expanded Dispatcher Logic
    if (strcmp(algo_name, "FCFS") == 0) {
        picker = pick_fcfs;
        is_preemptive = 0; // Run until finished
    } else if (strcmp(algo_name, "SJF") == 0) {
        picker = pick_sjf;
        is_preemptive = 0; // Run until finished
    } else if (strcmp(algo_name, "STCF") == 0) {
        picker = pick_stcf;
        is_preemptive = 1; // Check for shorter jobs every tick
    } else if (strcmp(algo_name, "RR") == 0) {
        set_rr_quantum(quantum); // Pass your parsed quantum to the RR module
        picker = pick_rr;
        is_preemptive = 1; // Check quantum every tick
    } else {
        fprintf(stderr, "Algorithm %s not recognized.\n", algo_name);
        free(processes);
        return 1;
    }

    printf("Starting %s Simulation...\n", algo_name);
    // 3. The Engine: This handles the "Run Time" and "Clock"
    run_simulation(processes, process_count, picker, is_preemptive);
    
    // 4. Post-Simulation Reporting
    calculate_metrics(processes, process_count);
    print_gantt_chart(10); // You can adjust the scale as needed

    free(processes);
    return 0;
}