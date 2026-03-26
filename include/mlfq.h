#ifndef MLFQ_H
#define MLFQ_H

#define MAX_LEVELS 10

// Give the struct a name: MLFQLevel
typedef struct MLFQLevel {
    int queue_id;
    int quantum;
    int allotment; 
} MLFQLevel;

// Give the struct a name: MLFQConfig
typedef struct MLFQConfig {
    MLFQLevel levels[MAX_LEVELS];
    int level_count;
    int boost_period;
} MLFQConfig;

MLFQConfig* load_mlfq_config(const char *filename);
int validate_mlfq_config(MLFQConfig *cfg);
void free_mlfq_config(MLFQConfig *cfg);

#endif