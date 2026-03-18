#ifndef METRICS_H
#define METRICS_H

#include "process.h"

// Forward declaration for the named struct in mlfq.h
struct MLFQConfig; 
typedef struct MLFQConfig MLFQConfig; 

// Added: Definition for SchedulingMetrics to fix "unknown type" error
typedef struct {
    char algo_name[20];
    double avg_turnaround_time;
    double avg_waiting_time;
    double avg_response_time;
} SchedulingMetrics;

// Standard metrics
void calculate_metrics(Process *processes, int count);
void display_comparison_table(SchedulingMetrics *results, int num_algos);

// MLFQ Analysis
void print_mlfq_behavior_report(Process *procs, int count, MLFQConfig *cfg);

#endif