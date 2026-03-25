#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/mlfq.h"

MLFQConfig* load_mlfq_config(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "\033[0;31m[ERROR]\033[0m Could not open MLFQ config: %s\n", filename);
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

    while (fgets(line, sizeof(line), file)) {
        line_num++;
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r' || line[0] == ' ') continue;

        if (strncmp(line, "Q", 1) == 0) {
            MLFQLevel q;
            // If this is NOT 3, it means there is garbage like 'asfdsf'
            if (sscanf(line, "Q%d %d %d", &q.queue_id, &q.quantum, &q.allotment) == 3) {
                if (config->level_count < 10) {
                    config->levels[config->level_count++] = q;
                }
            } else {
                fprintf(stderr, "\033[0;31m[FATAL]\033[0m Malformed MLFQ structure at line %d: %s", line_num, line);
                goto error_cleanup; 
            }
        } else if (strncmp(line, "BOOST_PERIOD", 12) == 0) {
            if (sscanf(line, "BOOST_PERIOD %d", &config->boost_period) != 1) {
                fprintf(stderr, "\033[0;31m[FATAL]\033[0m Invalid BOOST_PERIOD at line %d\n", line_num);
                goto error_cleanup;
            }
        }
    }

    fclose(file);

    if (config->level_count == 0 || config->boost_period <= 0) {
        fprintf(stderr, "\033[0;31m[ERROR]\033[0m MLFQ config is incomplete or missing BOOST_PERIOD.\n");
        free(config);
        return NULL;
    }

    return config;

error_cleanup:
    fclose(file);
    free(config);
    return NULL; // RETURN NULL TO SIGNAL FAILURE
}