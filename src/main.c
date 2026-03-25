#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scheduler.h"
#include "metrics.h"
#include "gantt.h"
#include "logger.h"
#include "mlfq.h"

int validate_mlfq_config(MLFQConfig *cfg) {
    if (!cfg) return 0;
    
    if (cfg->level_count <= 0) {
        fprintf(stderr, "Error: MLFQ config must have at least one queue level.\n");
        return 0;
    }
    
    if (cfg->boost_period <= 0) {
        fprintf(stderr, "Error: BOOST_PERIOD must be a positive integer.\n");
        return 0;
    }

    for (int i = 0; i < cfg->level_count; i++) {
        // Allotment consistency: Quantum must be <= Allotment (except for bottom queue)
        if (cfg->levels[i].allotment != -1 && cfg->levels[i].quantum > cfg->levels[i].allotment) {
            fprintf(stderr, "Error: Q%d quantum (%d) cannot be larger than its allotment (%d).\n", 
                    i, cfg->levels[i].quantum, cfg->levels[i].allotment);
            return 0;
        }
    }
    return 1;
}

int main(int argc, char *argv[]) {
    char *input_file = NULL;
    char *mlfq_file = "config/mlfq.conf"; // Default path
    char *algo_name = "FCFS"; 
    int quantum_arg = 10;   // renamed to avoid conflict with RR's internal variable

    // 1. Argument Parsing
    for (int i = 1; i < argc; i++) {
        if (strncmp(argv[i], "--input=", 8) == 0) {
            input_file = argv[i] + 8;
        } else if (strncmp(argv[i], "--algorithm=", 12) == 0) {
            algo_name = argv[i] + 12;
        } else if (strncmp(argv[i], "--quantum=", 10) == 0) {
            quantum_arg = atoi(argv[i] + 10);
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

    SchedulerConfig config = {0};
    AlgorithmPicker picker = NULL;
    int is_preemptive = 0;

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
        config.quantum = quantum_arg;
        picker = pick_rr;
        is_preemptive = 1;
    } else if (strcmp(algo_name, "MLFQ") == 0) {
        // Assign to the config pointer directly
        config.mlfq_cfg = load_mlfq_config(mlfq_file);

        if (!config.mlfq_cfg) {
            free(processes);
            return 1;
        }
        if (!validate_mlfq_config(config.mlfq_cfg)) {
            exit(EXIT_FAILURE);
        }
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

    // Analysis trigger
    if (strcmp(algo_name, "MLFQ") == 0) {
        print_mlfq_behavior_report(processes, process_count, config.mlfq_cfg);
    }

    // 5. Cleanup
    if (config.mlfq_cfg) free(config.mlfq_cfg);
    free(processes);
    
    return 0;
}