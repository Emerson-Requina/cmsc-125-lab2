#ifndef METRICS_H
#define METRICS_H

#include "process.h"

typedef struct {
    double avg_turnaround_time;
    double avg_waiting_time;
    double avg_response_time;
    int context_switches;
} SchedulingMetrics;

// Calculate TT, WT, and RT for each process
void calculate_metrics(Process *processes, int count);

// Print the comparative analysis table
void display_comparison_table(SchedulingMetrics *results, int num_algos);

#endif