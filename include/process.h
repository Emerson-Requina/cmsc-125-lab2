#ifndef PROCESS_H
#define PROCESS_H

typedef struct {
    char pid[16];           // Process identifier
    int arrival_time;       // When process arrives
    int burst_time;         // Total CPU time needed
    int remaining_time;     // For preemptive algorithms
    int start_time;         // When first executed (for RT)
    int finish_time;        // When completed (for TT)
    int waiting_time;       // Time spent waiting
    int responded;  
    // --- MLFQ Tracking Fields ---
    int priority;                 // Current level
    int time_in_queue;            // Current allotment tracker
    int lowest_priority_attained; // The max value 'priority' ever reached
    int demotion_time;        // Boolean flag to check if process has started yet
} Process;

// Load processes from a file (e.g., workload.txt)
Process* load_processes(const char *filename, int *count);

#endif