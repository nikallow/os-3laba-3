#ifndef LABA3_DETERMINANT_H
#define LABA3_DETERMINANT_H

#include "matrix.h"

#include <stdbool.h>

struct Calculation {
    int64_t determinant;
    size_t threads;
};

// The thread limit includes the caller. Matrix must be initialized and filled
bool determinant_compute(const struct Matrix *matrix, size_t thread_limit, struct Calculation *result);

#endif
