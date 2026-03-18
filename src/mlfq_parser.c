#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/mlfq.h"

MLFQConfig* load_mlfq_config(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening MLFQ config");
        return NULL;
    }

    MLFQConfig *config = malloc(sizeof(MLFQConfig));
    config->level_count = 0;
    config->boost_period = 0;

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        // Skip comments and empty lines
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') continue;

        if (strncmp(line, "Q", 1) == 0) {
            // Parse Queue Level: Q<ID> <Quantum> <Allotment>
            MLFQLevel q;
            if (sscanf(line, "Q%d %d %d", &q.queue_id, &q.quantum, &q.allotment) == 3) {
                config->levels[config->level_count++] = q;
            }
        } else if (strncmp(line, "BOOST_PERIOD", 12) == 0) {
            // Parse Boost Period: BOOST_PERIOD <Value>
            sscanf(line, "BOOST_PERIOD %d", &config->boost_period);
        }
    }

    fclose(file);
    return config;
}