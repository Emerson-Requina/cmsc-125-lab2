#ifndef GANTT_H
#define GANTT_H

#include "process.h"

// Log an execution event (Process X ran from Time A to Time B)
void log_execution(const char *pid, int start, int end);

// Generate the ASCII Gantt chart after simulation
void print_gantt_chart(int scale);

#endif