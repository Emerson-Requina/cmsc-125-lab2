#ifndef LOGGER_H
#define LOGGER_H

void log_step(int time, const char* pid, const char* status);
void log_arrival(int time, const char* pid);
void log_preemption(int time, const char* old_pid, const char* new_pid);
void log_gantt(const char* pid, int start, int end);

#endif