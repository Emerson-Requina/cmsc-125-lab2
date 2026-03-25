#ifndef MLFQ_H
#define MLFQ_H

#include "process.h"

typedef struct {
    int queue_id;
    int quantum;
    int allotment;
} MLFQLevel;

// Added 'MLFQConfig' here to give the struct a name
typedef struct MLFQConfig { 
    MLFQLevel levels[10];
    int level_count;
    int boost_period;
} MLFQConfig;

MLFQConfig* load_mlfq_config(const char *filename);
void set_mlfq_config(MLFQConfig *cfg);
void free_mlfq_config(MLFQConfig *cfg);
#endif