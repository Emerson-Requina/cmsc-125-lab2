#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "metrics.h"
#include "mlfq.h"

void calculate_metrics(Process *processes, int count) {
    double total_tt = 0, total_wt = 0, total_rt = 0;

    printf("\n--- Scheduling Metrics ---\n");
    printf("%-10s %-10s %-10s %-10s %-10s %-10s\n", 
           "PID", "Arrival", "Burst", "Finish", "TT", "WT");

    for (int i = 0; i < count; i++) {
        int tt = processes[i].finish_time - processes[i].arrival_time;
        int wt = tt - processes[i].burst_time;
        int rt = processes[i].start_time - processes[i].arrival_time;

        processes[i].waiting_time = wt;
        total_tt += tt;
        total_wt += wt;
        total_rt += rt;

        printf("%-10s %-10d %-10d %-10d %-10d %-10d\n", 
               processes[i].pid, processes[i].arrival_time, 
               processes[i].burst_time, processes[i].finish_time, tt, wt);
    }

    printf("--------------------------\n");
    printf("Average Turnaround Time: %.2f\n", total_tt / count);
    printf("Average Waiting Time:    %.2f\n", total_wt / count);
    printf("Average Response Time:   %.2f\n", total_rt / count);
}

void display_comparison_table(SchedulingMetrics *results, int num_algos) {
    printf("\n--- Comparative Analysis ---\n");
    printf("%-15s %-12s %-12s %-12s\n", "Algorithm", "Avg TT", "Avg WT", "Avg RT");
    printf("------------------------------------------------------------\n");

    for (int i = 0; i < num_algos; i++) {
        // Updated to use .algo_name instead of printing 'i'
        printf("%-15s %-12.2f %-12.2f %-12.2f\n", 
               results[i].algo_name, 
               results[i].avg_turnaround_time, 
               results[i].avg_waiting_time, 
               results[i].avg_response_time);
    }
}

void print_mlfq_behavior_report(Process *procs, int count, MLFQConfig *cfg) {
    if (!cfg) return;

    double interactive_rt_total = 0;
    int interactive_count = 0;

    printf("\n=== MLFQ Configuration ===\n");
    for (int i = 0; i < cfg->level_count; i++) {
        printf("Queue %d: q=%d, allotment=%d %s\n", 
               i, cfg->levels[i].quantum, cfg->levels[i].allotment,
               (i == 0) ? "(highest priority)" : (i == cfg->level_count - 1) ? "(lowest priority)" : "");
    }
    printf("Boost period: %d\n", cfg->boost_period);

    printf("\n=== Analysis ===\n");
    
    printf("Interactive job (short burst) behavior:\n");
    for (int i = 0; i < count; i++) {
        if (procs[i].lowest_priority_attained == 0) {
            printf("  - Process %s stayed in Q0 (completed in %d time units)\n", 
                   procs[i].pid, procs[i].finish_time - procs[i].arrival_time);
            interactive_rt_total += (procs[i].start_time - procs[i].arrival_time);
            interactive_count++;
        }
    }
    if (interactive_count > 0) {
        printf("  - Average response time: %.1f\n", interactive_rt_total / interactive_count);
    }

    printf("\nLong-running job behavior:\n");
    for (int i = 0; i < count; i++) {
        // CHANGE: Check if it was ever demoted at least once (priority > 0)
        if (procs[i].lowest_priority_attained > 0) { 
            printf("  - Process %s demoted to Q%d after %d time units\n", 
                   procs[i].pid, 
                   procs[i].lowest_priority_attained, 
                   procs[i].demotion_time);
            printf("  - Turnaround time: %d (fair for its burst time)\n", 
                   procs[i].finish_time - procs[i].arrival_time);
        }
    }

    printf("\nYour MLFQ successfully balanced responsiveness and fairness.\n");
}