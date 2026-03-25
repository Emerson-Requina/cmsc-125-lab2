#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <ctype.h>
#include "scheduler.h"
#include "metrics.h"
#include "gantt.h"
#include "logger.h"
#include "mlfq.h"

// Helper to normalize algorithm names (e.g.,"rr" -> "RR")
void str_toupper(char* str) {
    for (; *str; ++str) *str = toupper(*str);
}

// Deep copy the process array to prevent mutating the original
Process* clone_processes(Process* original, int count) {
    if (!original) return NULL;
    Process* copy = malloc(sizeof(Process) * count);
    if (!copy) return NULL;
    
    // Perform a bitwise copy of the entire array
    memcpy(copy, original, sizeof(Process) * count);
    return copy;
}

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

    // CLI Parsing with getopt_long
    Process *base_processes = NULL;
    Process *work_processes = NULL;
    MLFQConfig *mlfq_cfg = NULL;
    int process_count = 0;
    int exit_code = 0;
    
    static struct option long_options[] = {
        {"algorithm",    required_argument, 0, 'a'},
        {"input",        required_argument, 0, 'i'},
        {"quantum",      required_argument, 0, 'q'},
        {"mlfq-config",  required_argument, 0, 'm'},
        {0, 0, 0, 0}
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "a:i:q:m:", long_options, NULL)) != -1) {
        char *endptr;
        switch (opt) {
            case 'a':
                algo_name = optarg;
                str_toupper(algo_name); // Normalize to uppercase
                break;
            case 'i':
                input_file = optarg;
                break;
            case 'q':
                quantum_arg = (int)strtol(optarg, &endptr, 10);
                if (*endptr != '\0' || quantum_arg <= 0) {
                    fprintf(stderr, "Error: Invalid quantum '%s'. Must be positive integer.\n", optarg);
                    return 1;
                }
                break;
            case 'm':
                mlfq_file = optarg;
                break;
            default:
                fprintf(stderr, "Usage: %s --algorithm=<ALGO> --input=<FILE> [--quantum=<N>] [--mlfq-config=<FILE>]\n", argv[0]);
                return 1;
        }
    }

    if (!input_file) {
        fprintf(stderr, "Usage: %s --algorithm=<ALGO> --input=<FILE> [--quantum=<INT>] [--mlfq-config=<FILE>]\n", argv[0]);
        return 1;
    }

    // Resource Loading
    base_processes = load_processes(input_file, &process_count);
    if (!base_processes || process_count == 0) {
        exit_code = 1;
        goto cleanup;
    }

    // Deep copy for the actual simulation run
    work_processes = clone_processes(base_processes, process_count);

    SchedulerConfig config = {0};
    AlgorithmPicker picker = NULL;
    int is_preemptive = 0;

    // 2. Expanded Dispatcher Logic
    if (strcmp(algo_name, "FCFS") == 0) {
        picker = pick_fcfs;
    } else if (strcmp(algo_name, "SJF") == 0) {
        picker = pick_sjf;
    } else if (strcmp(algo_name, "STCF") == 0) {
        picker = pick_stcf;
        is_preemptive = 1;
    } else if (strcmp(algo_name, "RR") == 0) {
        config.quantum = quantum_arg;
        picker = pick_rr;
        is_preemptive = 1;
    } else if (strcmp(algo_name, "MLFQ") == 0) {
        // Assign to the config pointer directly
        mlfq_cfg = load_mlfq_config(mlfq_file);

        if (!mlfq_cfg || !validate_mlfq_config(mlfq_cfg)) {
            exit_code = 1;
            goto cleanup;
        }
        config.mlfq_cfg = mlfq_cfg;
        picker = pick_mlfq;
        is_preemptive = 1; // MLFQ must be preemptive to handle quantums/boosts
    } else {
        fprintf(stderr, "Algorithm %s not recognized.\n", algo_name);
        exit_code = 1;
        goto cleanup;
    }

    printf("Starting %s Simulation...\n", algo_name);
    
    // 3. The Engine
    run_simulation(work_processes, process_count, picker, is_preemptive, &config);

    // 4. Post-Simulation Reporting
    calculate_metrics(work_processes, process_count);
    print_gantt_chart(1); // Micro-gantt call

    // Analysis trigger
    if (strcmp(algo_name, "MLFQ") == 0) {
        print_mlfq_behavior_report(work_processes, process_count, mlfq_cfg);
    }

    // 5. Cleanup
    cleanup:
        if (base_processes) free(base_processes);
        if (work_processes) free(work_processes);
        if (mlfq_cfg) free(mlfq_cfg); // Ensure MLFQ library has a way to free nested levels if needed
    
    return exit_code;
}