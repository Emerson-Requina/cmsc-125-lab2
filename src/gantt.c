#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/gantt.h"

typedef struct {
    char pid[16];
    int start;
    int end;
} GanttEvent;

static GanttEvent events[1000]; // Fixed size for simplicity
static int event_count = 0;

void log_execution(const char *pid, int start, int end) {
    if (event_count < 1000) {
        strncpy(events[event_count].pid, pid, 15);
        events[event_count].start = start;
        events[event_count].end = end;
        event_count++;
    }
}

void print_gantt_chart(int scale) {
    printf("\n--- Gantt Chart ---\n");
    
    // Top border
    for (int i = 0; i < event_count; i++) printf("----------");
    printf("\n|");

    // PID Row
    for (int i = 0; i < event_count; i++) {
        printf("  %s  |", events[i].pid);
    }
    printf("\n");

    // Bottom border
    for (int i = 0; i < event_count; i++) printf("----------");
    printf("\n0");

    // Time Row
    for (int i = 0; i < event_count; i++) {
        printf("%10d", events[i].end);
    }
    printf("\n\n(Scale: %d units per block)\n", scale);
}