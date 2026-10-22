#include "permutation.h"

static void exchange(size_t *a, size_t *b) {
    const size_t temp = *a;
    *a = *b;
    *b = temp;
}

bool permutation_next(size_t *values, size_t count, bool *negative) {
    if (count < 2) return false;

    size_t suffix = count - 1;
    while (suffix > 0 && values[suffix - 1] >= values[suffix]) {
        suffix--;
    }

    if (suffix == 0) return false;

    size_t pivot = suffix - 1;

    size_t replacement = count - 1;
    while (values[replacement] <= values[pivot]) {
        replacement--;
    }

    exchange(&values[pivot], &values[replacement]);
    *negative = !(*negative);

    size_t left = suffix;
    size_t right = count - 1;

    while (left < right) {
        exchange(&values[left], &values[right]);
        *negative = !(*negative);
        left++;
        right--;
    }

    return true;
}
