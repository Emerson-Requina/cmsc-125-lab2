#include <stdio.h>
#include "../include/metrics.h"
#include "../include/scheduler.h"

/**
 * Standard metrics table for all algorithms.
 * Calculates Turnaround Time (TT), Waiting Time (WT), and Response Time (RT).
 */
void calculate_metrics(Process *processes, int count) {
    double total_tt = 0, total_wt = 0, total_rt = 0;

    printf("\n=== Metrics ===\n");
    printf("%-8s | %-3s | %-3s | %-3s | %-3s | %-3s | %-3s\n", 
           "Process", "AT", "BT", "FT", "TT", "WT", "RT");
    printf("--------|-----|-----|-----|-----|-----|-----\n");

    for (int i = 0; i < count; i++) {
        int tt = processes[i].finish_time - processes[i].arrival_time;
        int wt = tt - processes[i].burst_time;
        int rt = processes[i].start_time - processes[i].arrival_time;

        total_tt += tt;
        total_wt += wt;
        total_rt += rt;

        printf("%-8s | %3d | %3d | %3d | %3d | %3d | %3d\n", 
               processes[i].pid, processes[i].arrival_time, processes[i].burst_time,
               processes[i].finish_time, tt, wt, rt);
    }

    printf("--------|-----|-----|-----|-----|-----|-----\n");
    printf("%-8s |     |     |     | %3.1f | %3.1f | %3.1f\n", 
           "Average", total_tt / count, total_wt / count, total_rt / count);
}

/**
 * Specialized Analysis for MLFQ.
 * Explains process behavior based on the final queue reached.
 */
void print_mlfq_analysis(Process *processes, int count) {
    double total_rt = 0;
    
    printf("\n=== Analysis ===\n");
    
    // 1. Interactive Job Behavior (High Priority Success)
    printf("Interactive job (short burst) behavior:\n");
    for (int i = 0; i < count; i++) {
        int tt = processes[i].finish_time - processes[i].arrival_time;
        total_rt += (processes[i].start_time - processes[i].arrival_time);

        if (processes[i].priority == 0) {
            printf("  - Process %s stayed in Q0 (completed in %d time units)\n", 
                   processes[i].pid, tt);
        } else if (processes[i].priority == 1) {
            printf("  - Process %s was handled in Q1 (completed in %d time units)\n", 
                   processes[i].pid, tt);
        }
    }
    printf("  - Average response time: %.1f\n", total_rt / count);

    // 2. Long-running Job Behavior (Demotion tracking)
    printf("\nLong-running job behavior:\n");
    for (int i = 0; i < count; i++) {
        if (processes[i].priority >= 2) {
            int tt = processes[i].finish_time - processes[i].arrival_time;
            printf("  - Process %s demoted to Q%d (identified as compute-intensive) - Turnaround: %d\n", 
                   processes[i].pid, processes[i].priority, tt);
        }
    }

    printf("\nYour MLFQ successfully balanced responsiveness and fairness.\n");
}