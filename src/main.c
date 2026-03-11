#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/scheduler.h"
#include "../include/metrics.h"
#include "../include/gantt.h"

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

    printf("Running %s Scheduler...\n", algo_name);

    // 2. Expanded Dispatcher Logic
    if (strcmp(algo_name, "FCFS") == 0) {
        run_fcfs(processes, process_count);
    } else if (strcmp(algo_name, "RR") == 0) {
        printf("Using Quantum: %d\n", quantum);
        run_rr(processes, process_count, quantum);
    } else if (strcmp(algo_name, "SJF") == 0) {
        run_sjf(processes, process_count);
    } else if (strcmp(algo_name, "STCF") == 0) {
        run_stcf(processes, process_count);
    } else {
        printf("Algorithm %s not yet implemented.\n", algo_name);
        free(processes);
        return 1;
    }

    // 3. Post-Simulation Reporting
    calculate_metrics(processes, process_count);
    print_gantt_chart(10); // You can adjust the scale as needed

    free(processes);
    return 0;
}