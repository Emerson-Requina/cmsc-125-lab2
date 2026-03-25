#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gantt.h"

#define FIXED_WIDTH 6
#define MAX_BLOCKS_PER_LINE 10  // Adjust this based on your terminal width

static GanttEvent events[1000]; 
static int event_count = 0;

void log_execution(const char *pid, int start, int end) {
    if (start >= end) return; 
    if (event_count < 1000) {
        strncpy(events[event_count].pid, pid, 15);
        events[event_count].pid[15] = '\0';
        events[event_count].start = start;
        events[event_count].end = end;
        event_count++;
    }
}

void print_gantt_chart(int scale) {
    if (event_count == 0) return;
    (void)scale;

    printf("\n--- Multi-Level Gantt Chart ---\n");

    int processed = 0;
    while (processed < event_count) {
        int line_end = processed + MAX_BLOCKS_PER_LINE;
        if (line_end > event_count) line_end = event_count;

        // 1. Top Border for the current line
        for (int i = processed; i < line_end; i++) {
            printf("+");
            for (int j = 0; j < FIXED_WIDTH; j++) printf("-");
        }
        printf("+\n|");

        // 2. PID Row for the current line
        for (int i = processed; i < line_end; i++) {
            int text_len = (int)strlen(events[i].pid);
            int print_len = (text_len > FIXED_WIDTH) ? FIXED_WIDTH : text_len;
            int padding = (FIXED_WIDTH - print_len) / 2;

            for (int j = 0; j < padding; j++) printf(" ");
            printf("%.*s", print_len, events[i].pid);
            for (int j = 0; j < (FIXED_WIDTH - print_len - padding); j++) printf(" ");
            printf("|");
        }
        printf("\n");

        // 3. Bottom Border for the current line
        for (int i = processed; i < line_end; i++) {
            printf("+");
            for (int j = 0; j < FIXED_WIDTH; j++) printf("-");
        }
        printf("+\n");

        // 4. Time Labels for the current line
        printf("%d", events[processed].start);
        for (int i = processed; i < line_end; i++) {
            char end_str[16];
            sprintf(end_str, "%d", events[i].end);
            int spacing = FIXED_WIDTH - (int)strlen(end_str) + 1;
            
            for (int j = 0; j < (spacing > 0 ? spacing : 1); j++) printf(" ");
            printf("%s", end_str);
        }
        printf("\n\n"); // Extra newline between levels

        processed = line_end;
    }
}