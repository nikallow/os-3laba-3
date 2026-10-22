#include "determinant.h"
#include "permutation.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Worker {
    const struct Matrix *matrix;
    size_t index;
    size_t thread_count;
    size_t job_count;
    int64_t *sums;
    bool success;
};

static bool prepare_permutation(size_t *permutation, size_t size, size_t job) {
    if (size == 1) {
        permutation[0] = 0;
        return false;
    }

    permutation[0] = job / (size - 1);
    permutation[1] = job % (size - 1);
    if (permutation[1] >= permutation[0]) {
        permutation[1]++;
    }

    size_t position = 2;
    for (size_t column = 0; column < size; column++) {
        if (column != permutation[0] && column != permutation[1]) {
            permutation[position++] = column;
        }
    }

    // Each out-of-order pair changes the sign of the permutation
    bool negative = false;
    for (size_t left = 0; left < size; left++) {
        for (size_t right = left + 1; right < size; right++) {
            if (permutation[left] > permutation[right]) {
                negative = !negative;
            }
        }
    }
    return negative;
}

static void *worker_run(void *argument) {
    struct Worker *worker = argument;
    const size_t size = worker->matrix->size;
    const size_t prefix = size == 1 ? 1 : 2;
    size_t *permutation = malloc(size * sizeof(*permutation));
    if (permutation == NULL) {
        perror("Cannot allocate permutation");
        worker->success = false;
        return NULL;
    }

    for (size_t job = worker->index; job < worker->job_count && worker->success; job += worker->thread_count) {
        bool negative = prepare_permutation(permutation, size, job);
        int64_t sum = 0;

        do {
            int64_t product = 1;
            for (size_t row = 0; row < size; row++) {
                int64_t element = worker->matrix->data[row * size + permutation[row]];
                if (__builtin_mul_overflow(product, element, &product)) {
                    worker->success = false;
                    break;
                }
            }
            if (!worker->success) {
                break;
            }

            bool overflow = negative
                                ? __builtin_sub_overflow(sum, product, &sum)
                                : __builtin_add_overflow(sum, product, &sum);
            if (overflow) {
                worker->success = false;
                break;
            }
        } while (permutation_next(permutation + prefix, size - prefix, &negative));

        if (worker->success) {
            worker->sums[job] = sum;
        }
    }

    if (!worker->success) {
        fprintf(stderr, "Integer overflow during calculation\n");
    }
    free(permutation);
    return NULL;
}

bool determinant_compute(const struct Matrix *matrix, size_t thread_limit, struct Calculation *result) {
    if (matrix == NULL || matrix->data == NULL || matrix->size == 0 || thread_limit == 0 || result == NULL) {
        fprintf(stderr, "Invalid calculation arguments\n");
        return false;
    }

    const size_t size = matrix->size;
    const size_t jobs = size == 1 ? 1 : size * (size - 1); // count amount of jobs
    const size_t count = thread_limit < jobs ? thread_limit : jobs; // min

    if (jobs > SIZE_MAX / sizeof(int64_t) || count > SIZE_MAX / sizeof(struct Worker) || count - 1 > SIZE_MAX / sizeof(
            pthread_t) || size > SIZE_MAX / sizeof(size_t)) {
        fprintf(stderr, "Calculation allocation size overflow\n");
        return false;
    }

    int64_t *sums = malloc(jobs * sizeof(*sums));
    struct Worker *workers = malloc(count * sizeof(*workers));
    pthread_t *threads = count > 1 ? malloc((count - 1) * sizeof(*threads)) : NULL;
    bool success = sums != NULL && workers != NULL && (count == 1 || threads != NULL);

    if (!success) {
        fprintf(stderr, "Cannot allocate calculation memory\n");
        // free memory after this if-else
    } else {
        for (size_t index = 0; index < count; index++) {
            workers[index] = (struct Worker){
                .matrix = matrix, .index = index, .thread_count = count,
                .job_count = jobs, .sums = sums, .success = true
            };
        }

        size_t created = 0;

        for (size_t index = 1; index < count; index++) {
            int code = pthread_create(&threads[created], NULL, worker_run, &workers[index]);

            if (code != 0) {
                fprintf(stderr, "pthread_create: %s\n", strerror(code));
                success = false;
                break;
            }

            created++;
        }

        if (success) {
            worker_run(&workers[0]);
        }

        for (size_t index = 0; index < created; index++) {
            int code = pthread_join(threads[index], NULL);

            if (code != 0) {
                fprintf(stderr, "pthread_join: %s\n", strerror(code));
                fflush(stderr);

                // An unjoined worker may still use the matrix and these arrays
                _Exit(EXIT_FAILURE);
            }
        }

        for (size_t index = 0; index < count; index++) {
            if (!workers[index].success) {
                success = false;
            }
        }

        int64_t sum = 0;
        if (success) {
            for (size_t job = 0; job < jobs; job++) {
                if (__builtin_add_overflow(sum, sums[job], &sum)) {
                    fprintf(stderr, "Integer overflow while adding partial sums\n");
                    success = false;
                    break;
                }
            }
        }

        if (success) {
            *result = (struct Calculation){.determinant = sum, .threads = count};
        }
    }

    free(threads);
    free(workers);
    free(sums);

    return success;
}
