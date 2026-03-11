#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/scheduler.h"

MLFQConfig parse_mlfq_config(const char *filename) {
    MLFQConfig config = { .num_queues = 0, .boost_period = 0 };
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open config file");
        exit(1);
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        // Skip comments and empty lines
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') continue;

        if (strncmp(line, "Q", 1) == 0) {
            int id, q, a;
            if (sscanf(line, "Q%d %d %d", &id, &q, &a) == 3) {
                config.queues[config.num_queues].id = id;
                config.queues[config.num_queues].quantum = q;
                config.queues[config.num_queues].allotment = a;
                config.num_queues++;
            }
        } else if (strncmp(line, "BOOST_PERIOD", 12) == 0) {
            sscanf(line, "BOOST_PERIOD %d", &config.boost_period);
        }
    }

    fclose(file);
    return config;
}