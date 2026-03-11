#include <stdio.h>
#include <string.h>
#include "../include/gantt.h"

typedef struct {
    char pid[16];
    int start;
    int end;
} GanttEvent;

static GanttEvent events[1000];
static int event_count = 0;

void log_execution(const char *pid, int start, int end) {
    if (event_count < 1000) {
        strncpy(events[event_count].pid, pid, 15);
        events[event_count].start = start;
        events[event_count].end = end;
        event_count++;
    }
}

void print_gantt_chart(int scale_factor) {
    if (scale_factor <= 0) scale_factor = 10; // Default scaling
    
    printf("\n=== Gantt Chart ===\n");

    // 1. Print the PID row with relative scaling
    for (int i = 0; i < event_count; i++) {
        int duration = events[i].end - events[i].start;
        int num_dashes = duration / scale_factor;
        if (num_dashes < 1) num_dashes = 1; // Ensure at least 1 dash for tiny bursts

        printf("[%s", events[i].pid);
        for (int j = 0; j < num_dashes; j++) {
            printf("-");
        }
        printf("]");
    }
    printf("\n");

    // 2. Print the Time markers aligned with the edges
    printf("%-3d", events[0].start);
    for (int i = 0; i < event_count; i++) {
        int duration = events[i].end - events[i].start;
        int num_dashes = duration / scale_factor;
        if (num_dashes < 1) num_dashes = 1;

        // Total width of bracket is PID length + dashes + 2 brackets
        // We use %*d to dynamically set the width for alignment
        int field_width = strlen(events[i].pid) + num_dashes + 1;
        printf("%*d", field_width, events[i].end);
    }
    printf("\n");
}