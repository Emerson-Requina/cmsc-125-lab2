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
#include "comparison.h"

// Helper to normalize algorithm names
void str_toupper(char* str) {
    for (; *str; ++str) *str = toupper(*str);
}

// Deep copy to prevent mutating the original during comparison runs
Process* clone_processes(Process* original, int count) {
    if (!original) return NULL;
    Process* copy = malloc(sizeof(Process) * count);
    if (!copy) return NULL;
    memcpy(copy, original, sizeof(Process) * count);
    
    // NOTE: If Process had heap-allocated strings, you would 
    // need to strdup() them here for a true deep copy.
    return copy;
}

int validate_mlfq_config(MLFQConfig *cfg) {
    if (!cfg) return 0;
    if (cfg->level_count <= 0 || cfg->boost_period <= 0) return 0;
    for (int i = 0; i < cfg->level_count; i++) {
        if (cfg->levels[i].allotment != -1 && cfg->levels[i].quantum > cfg->levels[i].allotment) {
            fprintf(stderr, "Error: Invalid allotment/quantum ratio in Q%d\n", i);
            return 0;
        }
    }
    return 1;
}

int main(int argc, char *argv[]) {
    char *input_file = NULL;
    char *mlfq_file = "config/mlfq.conf"; 
    char *algo_name = "FCFS"; 
    int quantum_arg = 10;
    int compare_mode = 0;

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
        {"compare",      no_argument,       0, 'c'},
        {0, 0, 0, 0}
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "a:i:q:m:c", long_options, NULL)) != -1) {
        char *endptr;
        switch (opt) {
            case 'a':
                algo_name = optarg;
                str_toupper(algo_name);
                break;
            case 'i':
                input_file = optarg;
                break;
            case 'c':
                compare_mode = 1;
                break;
            case 'q':
                quantum_arg = (int)strtol(optarg, &endptr, 10);
                if (*endptr != '\0' || quantum_arg <= 0) {
                    fprintf(stderr, "Error: Invalid quantum.\n");
                    return 1;
                }
                break;
            case 'm':
                mlfq_file = optarg;
                break;
            default:
                return 1;
        }
    }

    if (!input_file) {
        fprintf(stderr, "Usage: %s --input=<FILE> [--compare]\n", argv[0]);
        return 1;
    }

    // 1. Load original process list
    base_processes = load_processes(input_file, &process_count);
    if (!base_processes || process_count == 0) return 1;

    // --- LOGIC SPLIT: COMPARISON VS SINGLE RUN ---
    if (compare_mode) {
        printf("Starting Comparative Analysis Mode...\n");
        // Ownership: main owns base_processes, comparison function creates its own clones
        run_comparative_analysis(base_processes, process_count, mlfq_file);
        goto cleanup; 
    }

    // 2. Prepare for single simulation
    work_processes = clone_processes(base_processes, process_count);
    SchedulerConfig config = {0};
    AlgorithmPicker picker = NULL;
    int is_preemptive = 0;

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
        mlfq_cfg = load_mlfq_config(mlfq_file);
        if (!mlfq_cfg || !validate_mlfq_config(mlfq_cfg)) {
            exit_code = 1;
            goto cleanup;
        }
        config.mlfq_cfg = mlfq_cfg; // Registration (Ownership remains in main)
        picker = pick_mlfq;
        is_preemptive = 1; 
    }

    printf("Starting %s Simulation...\n", algo_name);
    run_simulation(work_processes, process_count, picker, is_preemptive, &config);
    
    // 3. Post-simulation Reporting
    calculate_metrics(work_processes, process_count);
    print_gantt_chart(1);

    if (strcmp(algo_name, "MLFQ") == 0) {
        print_mlfq_behavior_report(work_processes, process_count, mlfq_cfg);
    }

cleanup:
    // --- 4. EXPLICIT CLEANUP (Fixes Memory Ownership & Leaks) ---
    
    // Deep free the original process list
    if (base_processes) {
        free_processes(base_processes);
    }

    // Deep free the mutated process list used in the simulation
    if (work_processes) {
        free_processes(work_processes);
    }

    // Deep free the MLFQ configuration
    if (mlfq_cfg) {
        free_mlfq_config(mlfq_cfg);
    }
    
    return exit_code;
}