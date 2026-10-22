#ifndef LABA3_MATRIX_H
#define LABA3_MATRIX_H

#include <stddef.h>
#include <stdint.h>

struct Matrix {
    size_t size;
    int64_t *data;
};

enum MatrixStatus {
    MATRIX_OK = 0,
    MATRIX_INVALID_ARGUMENT,
    MATRIX_INVALID_SIZE,
    MATRIX_SIZE_OVERFLOW,
    MATRIX_NO_MEMORY,
    MATRIX_INVALID_ELEMENT,
    MATRIX_MISSING_ELEMENT,
    MATRIX_EXTRA_DATA,
    MATRIX_READ_ERROR
};

// Requires an empty matrix initialized with {0}
enum MatrixStatus matrix_init(struct Matrix *matrix, size_t size);

void matrix_destroy(struct Matrix *matrix);

const char *matrix_status_string(enum MatrixStatus status);

#endif
