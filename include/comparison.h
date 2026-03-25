#ifndef COMPARISON_H
#define COMPARISON_H

#include "process.h"
#include "metrics.h"

// Function to run all algorithms and print a summary table
void run_comparative_analysis(Process *original_procs, int count, const char *mlfq_conf);

#endif