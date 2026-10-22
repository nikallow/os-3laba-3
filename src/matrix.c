#include "matrix.h"

#include <stdlib.h>

enum MatrixStatus matrix_init(struct Matrix *matrix, const size_t size) {
    if (matrix == NULL || matrix->data != NULL || matrix->size != 0) {
        return MATRIX_INVALID_ARGUMENT;
    }

    if (size == 0) {
        return MATRIX_INVALID_SIZE;
    }

    if (size > SIZE_MAX / size) {
        return MATRIX_SIZE_OVERFLOW;
    }

    const size_t count = size * size;
    if (count > SIZE_MAX / sizeof(*matrix->data)) {
        return MATRIX_SIZE_OVERFLOW;
    }

    int64_t *data = malloc(count * sizeof(*data));
    if (data == NULL) {
        return MATRIX_NO_MEMORY;
    }

    matrix->size = size;
    matrix->data = data;

    return MATRIX_OK;
}

void matrix_destroy(struct Matrix *matrix) {
    if (matrix == NULL) {
        return;
    }

    free(matrix->data);

    matrix->data = NULL;
    matrix->size = 0;
}

const char *matrix_status_string(const enum MatrixStatus status) {
    switch (status) {
        case MATRIX_OK:
            return "success";
        case MATRIX_INVALID_ARGUMENT:
            return "invalid argument or nonempty output matrix";
        case MATRIX_INVALID_SIZE:
            return "matrix size must be a positive decimal integer";
        case MATRIX_SIZE_OVERFLOW:
            return "matrix size exceeds the supported range";
        case MATRIX_NO_MEMORY:
            return "cannot allocate matrix memory";
        case MATRIX_INVALID_ELEMENT:
            return "invalid int64 matrix element";
        case MATRIX_MISSING_ELEMENT:
            return "not enough matrix elements";
        case MATRIX_EXTRA_DATA:
            return "extra data after matrix elements";
        case MATRIX_READ_ERROR:
            return "input read error";
    }

    return "unknown matrix status";
}
