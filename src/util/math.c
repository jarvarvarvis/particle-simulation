#include "math.h"

#include <stdlib.h>

float randf() {
    return (float)rand() / RAND_MAX;
}

float time_diff_ms(struct timespec t0, struct timespec t1) {
    return (t1.tv_sec - t0.tv_sec) * 1000.0f + (t1.tv_nsec - t0.tv_nsec) / 1000000.0f;
}
