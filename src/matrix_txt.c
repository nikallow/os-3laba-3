#include "matrix_txt.h"

enum MatrixStatus matrix_read_txt(FILE *input, struct Matrix *matrix) {
    if (input == NULL || matrix == NULL) {
        return MATRIX_INVALID_ARGUMENT;
    }
    if (matrix->data != NULL || matrix->size != 0) {
        return MATRIX_INVALID_ARGUMENT;
    }

    int64_t size;
    if (fscanf(input, "%ld", &size) != 1) {
        return ferror(input) ? MATRIX_READ_ERROR : MATRIX_INVALID_SIZE;
    }
    if (size <= 0) {
        return MATRIX_INVALID_SIZE;
    }

    struct Matrix temporary = {0};
    enum MatrixStatus status = matrix_init(&temporary, (size_t) size);
    if (status != MATRIX_OK) {
        return status;
    }

    const size_t count = temporary.size * temporary.size;
    for (size_t index = 0; index < count; index++) {
        int result = fscanf(input, "%ld", &temporary.data[index]);
        if (result != 1) {
            matrix_destroy(&temporary);
            if (ferror(input)) {
                return MATRIX_READ_ERROR;
            }
            return result == EOF ? MATRIX_MISSING_ELEMENT : MATRIX_INVALID_ELEMENT;
        }
    }

    char extra;
    int result = fscanf(input, " %c", &extra);
    if (ferror(input)) {
        matrix_destroy(&temporary);
        return MATRIX_READ_ERROR;
    }
    if (result == 1) {
        matrix_destroy(&temporary);
        return MATRIX_EXTRA_DATA;
    }

    *matrix = temporary;
    return MATRIX_OK;
}
