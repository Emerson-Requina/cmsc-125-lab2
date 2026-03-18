#ifndef GANTT_H
#define GANTT_H

typedef struct {
    char pid[16];
    int start;
    int end;
} GanttEvent;

void log_execution(const char *pid, int start, int end);
void print_gantt_chart(int scale);

#endif