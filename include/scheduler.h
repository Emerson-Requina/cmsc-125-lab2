#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "process.h"

// Define scheduling algorithm types
typedef enum {
    ALGO_FCFS,
    ALGO_SJF,
    ALGO_STCF,
    ALGO_RR,
    ALGO_MLFQ
} AlgoType;

typedef struct {
    int level;              // Queue priority level (0 = highest)
    int time_quantum;       // Time slice for this queue (-1 for FCFS)
    int allotment;          // Max time before demotion (-1 for infinite)
    Process *queue;         // Array or linked list of processes
    int size;               // Current queue size
} MLFQQueue;

typedef struct {
    MLFQQueue *queues;      // Array of queues
    int num_queues;         // Number of priority levels
    int boost_period;       // Period for priority boost (S)
    int last_boost;         // Last boost time
} MLFQScheduler;

// Main execution interfaces for algorithms
void run_fcfs(Process *processes, int count);
void run_sjf(Process *processes, int count);
void run_stcf(Process *processes, int count);
void run_rr(Process *processes, int count, int quantum);
void run_mlfq(Process *processes, int count, MLFQScheduler *sched);

#endif