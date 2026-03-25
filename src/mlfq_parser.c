#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mlfq.h"

MLFQConfig* load_mlfq_config(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "[ERROR] Could not open MLFQ config: %s\n", filename);
        return NULL;
    }

    MLFQConfig *config = malloc(sizeof(MLFQConfig));
    if (!config) {
        fclose(file);
        return NULL;
    }

    config->level_count = 0;
    config->boost_period = -1;

    char line[256];
    int line_num = 0;
    int error_found = 0;

    while (fgets(line, sizeof(line), file)) {
        line_num++;
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r' || line[0] == ' ') continue;

        if (strncmp(line, "Q", 1) == 0) {
            MLFQLevel q;
            if (sscanf(line, "Q%d %d %d", &q.queue_id, &q.quantum, &q.allotment) == 3) {
                if (config->level_count < MAX_LEVELS) {
                    config->levels[config->level_count++] = q;
                }
            } else {
                fprintf(stderr, "[FATAL] Malformed line %d\n", line_num);
                error_found = 1;
                break;
            }
        } else if (strncmp(line, "BOOST_PERIOD", 12) == 0) {
            if (sscanf(line, "BOOST_PERIOD %d", &config->boost_period) != 1) {
                fprintf(stderr, "[FATAL] Invalid BOOST_PERIOD at line %d\n", line_num);
                error_found = 1;
                break;
            }
        }
    }

    fclose(file);

    // If parsing failed OR the config is logically incomplete
    if (error_found || config->level_count == 0 || config->boost_period <= 0) {
        if (!error_found) {
            fprintf(stderr, "[ERROR] MLFQ config missing queues or BOOST_PERIOD.\n");
        }
        free(config);
        return NULL;
    }

    return config;
}
