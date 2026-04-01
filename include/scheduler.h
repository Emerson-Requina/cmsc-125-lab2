#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "process.h"
#include "mlfq.h" // Needed for the MLFQConfig type

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

// Config struct to pass parameters without globals
typedef struct {
    int quantum;            // For RR
    MLFQConfig *mlfq_cfg;   // For MLFQ 
} SchedulerConfig;

// typedef Process* (*AlgorithmPicker)(Process *procs, int count, int current_time, Process *current);
typedef Process* (*AlgorithmPicker)(Process *procs, int count, int time, Process *current, SchedulerConfig *config);

// The list of pick prototypes
Process* pick_fcfs(Process *procs, int count, int time, Process *current, SchedulerConfig *config);
Process* pick_sjf(Process *procs, int count, int time, Process *current, SchedulerConfig *config);
Process* pick_stcf(Process *procs, int count, int time, Process *current, SchedulerConfig *config);
Process* pick_rr(Process *procs, int count, int time, Process *current, SchedulerConfig *config);
Process* pick_mlfq(Process *procs, int count, int time, Process *current, SchedulerConfig *config);

void run_simulation(Process *procs, int count, AlgorithmPicker picker, int is_preemptive, SchedulerConfig *config);
void reset_scheduler_state();
#endif