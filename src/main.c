#include "determinant.h"
#include "matrix_txt.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static bool parse_threads(const char *text, size_t *count) {
    if (*text < '0' || *text > '9') {
        return false;
    }
    errno = 0;
    char *end = NULL;
    unsigned long value = strtoul(text, &end, 10);
    if (errno == ERANGE || *end != '\0' || value == 0) {
        return false;
    }
    *count = (size_t) value;
    return true;
}

int main(int argc, char *argv[]) {
    size_t thread_count;
    if (argc != 4 || strcmp(argv[1], "--threads") != 0 || !parse_threads(argv[2], &thread_count)) {
        fprintf(stderr, "Usage: %s --threads N matrix.txt\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE *input = fopen(argv[3], "r");
    if (input == NULL) {
        perror("Cannot open matrix file");
        return EXIT_FAILURE;
    }

    struct Matrix matrix = {0};
    enum MatrixStatus status = matrix_read_txt(input, &matrix);
    int close_result = fclose(input);

    bool success = status == MATRIX_OK && close_result == 0;
    if (close_result == EOF) {
        perror("Cannot close matrix file");
    }
    if (status != MATRIX_OK) {
        fprintf(stderr, "Cannot read matrix: %s\n", matrix_status_string(status));
    }

    struct timespec start = {0};
    struct timespec finish = {0};
    struct Calculation calculation = {0};

    if (success && clock_gettime(CLOCK_MONOTONIC, &start) == -1) {
        perror("clock_gettime(start)");
        success = false;
    }

    if (success) {
        success = determinant_compute(&matrix, thread_count, &calculation);
    }

    if (success && clock_gettime(CLOCK_MONOTONIC, &finish) == -1) {
        perror("clock_gettime(finish)");
        success = false;
    }

    if (success) {
        double elapsed = (double) (finish.tv_sec - start.tv_sec) + (double) (finish.tv_nsec - start.tv_nsec) / 1e9;

        if (printf("Determinant: %ld\nThreads: %zu\nTime: %.9f s\n", calculation.determinant, calculation.threads,
                   elapsed) < 0 || fflush(stdout) == EOF) {
            perror("Cannot write result");
            success = false;
        }
    }

    matrix_destroy(&matrix);

    return success ? EXIT_SUCCESS : EXIT_FAILURE;
}
