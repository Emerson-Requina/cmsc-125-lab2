#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/scheduler.h"
#include "../include/metrics.h"
#include "../include/gantt.h"

int main(int argc, char *argv[]) {
    char *input_file = NULL;
    char *algo_name = "FCFS"; 
    char *mlfq_config_file = "mlfq.conf"; // Default config path
    int quantum = 10;                     // Default for standard RR

    // 1. Argument Parsing
    for (int i = 1; i < argc; i++) {
        if (strncmp(argv[i], "--input=", 8) == 0) {
            input_file = argv[i] + 8;
        } else if (strncmp(argv[i], "--algorithm=", 12) == 0) {
            algo_name = argv[i] + 12;
        } else if (strncmp(argv[i], "--quantum=", 10) == 0) {
            quantum = atoi(argv[i] + 10);
        } else if (strncmp(argv[i], "--mlfq-config=", 14) == 0) {
            mlfq_config_file = argv[i] + 14;
        }
    }

    if (!input_file) {
        fprintf(stderr, "Usage: %s --algorithm=<ALGO> --input=<FILE> [--quantum=<INT>] [--mlfq-config=<FILE>]\n", argv[0]);
        return 1;
    }

    // 2. Load Process Workload
    int process_count = 0;
    Process *processes = load_processes(input_file, &process_count);

    if (!processes || process_count == 0) {
        fprintf(stderr, "Failed to load processes from %s\n", input_file);
        return 1;
    }

    // 3. Dispatcher Logic
    if (strcmp(algo_name, "MLFQ") == 0) {
        // Load Dynamic Config
        MLFQConfig config = parse_mlfq_config(mlfq_config_file);
        
        // Output Header for MLFQ
        printf("\n=== MLFQ Configuration ===\n");
        for (int i = 0; i < config.num_queues; i++) {
            if (config.queues[i].quantum == -1) {
                printf("Queue %d: FCFS (lowest priority)\n", i);
            } else {
                printf("Queue %d: q=%d, allotment=%d %s\n", 
                       i, config.queues[i].quantum, config.queues[i].allotment, 
                       (i == 0) ? "(highest priority)" : "");
            }
        }
        printf("Boost period: %d\n", config.boost_period);
        printf("\n=== Execution Trace ===\n");

        run_mlfq(processes, process_count, config);
        
        // Print the specialized MLFQ Analysis instead of Gantt
        print_mlfq_analysis(processes, process_count);

    } else {
        printf("Running %s Scheduler...\n", algo_name);

        if (strcmp(algo_name, "FCFS") == 0) {
            run_fcfs(processes, process_count);
        } else if (strcmp(algo_name, "RR") == 0) {
            run_rr(processes, process_count, quantum);
        } else if (strcmp(algo_name, "SJF") == 0) {
            run_sjf(processes, process_count);
        } else if (strcmp(algo_name, "STCF") == 0) {
            run_stcf(processes, process_count);
        } else {
            printf("Error: Algorithm %s not recognized.\n", algo_name);
            free(processes);
            return 1;
        }

        // Standard Output for non-MLFQ algorithms
        calculate_metrics(processes, process_count);
        print_gantt_chart(10); 
    }

    free(processes);
    return 0;
}