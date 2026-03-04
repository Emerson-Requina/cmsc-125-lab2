#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/scheduler.h"

int main(int argc, char *argv[]) {
    char *input_file = NULL;
    char *algo_name = "FCFS"; // Default 

    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (strncmp(argv[i], "--input=", 8) == 0) {
            input_file = argv[i] + 8;
        } else if (strncmp(argv[i], "--algorithm=", 12) == 0) {
            algo_name = argv[i] + 12;
        }
    }

    if (!input_file) {
        fprintf(stderr, "Usage: %s --algorithm=<ALGO> --input=<FILE>\n", argv[0]);
        return 1;
    }

    int process_count = 0;
    Process *processes = load_processes(input_file, &process_count);

    if (!processes || process_count == 0) {
        fprintf(stderr, "Failed to load processes from %s\n", input_file);
        return 1;
    }

    printf("Running %s Scheduler...\n", algo_name);

    // Dispatcher logic
    if (strcmp(algo_name, "FCFS") == 0) {
        run_fcfs(processes, process_count);
    } else {
        printf("Algorithm %s not yet implemented.\n", algo_name);
    }

    // After simulation, calculate and display results
    // calculate_metrics(processes, process_count);
    // print_gantt_chart(10);

    free(processes);
    return 0;
}