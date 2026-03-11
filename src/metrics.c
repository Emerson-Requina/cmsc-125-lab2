#include <stdio.h>
#include <stdlib.h>
#include "../include/metrics.h"

void calculate_metrics(Process *processes, int count) {
    double total_tt = 0, total_wt = 0, total_rt = 0;

    printf("\n--- Scheduling Metrics ---\n");
    printf("%-10s %-10s %-10s %-10s %-10s %-10s\n", 
           "PID", "Arrival", "Burst", "Finish", "TT", "WT");

    for (int i = 0; i < count; i++) {
        // Turnaround Time = Finish Time - Arrival Time
        int tt = processes[i].finish_time - processes[i].arrival_time;
        
        // Waiting Time = Turnaround Time - Burst Time
        // (This works for both preemptive and non-preemptive)
        int wt = tt - processes[i].burst_time;
        
        // Response Time = Start Time - Arrival Time
        int rt = processes[i].start_time - processes[i].arrival_time;

        // Store back in struct if needed for future analysis
        processes[i].waiting_time = wt;

        total_tt += tt;
        total_wt += wt;
        total_rt += rt;

        printf("%-10s %-10d %-10d %-10d %-10d %-10d\n", 
               processes[i].pid, 
               processes[i].arrival_time, 
               processes[i].burst_time, 
               processes[i].finish_time, 
               tt, wt);
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
        // Assuming your SchedulingMetrics struct has these fields 
        // as defined in your metrics.h
        printf("%-15d %-12.2f %-12.2f %-12.2f\n", 
               i, // You might want to add a name field to the struct later
               results[i].avg_turnaround_time, 
               results[i].avg_waiting_time, 
               results[i].avg_response_time);
    }
}