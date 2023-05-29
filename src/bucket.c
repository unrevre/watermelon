#include "bucket.h"

#include <stdlib.h>
#include <string.h>

struct bucket_t bucket;

void init_bucket(struct bucket_t* bucket, int32_t total) {
    bucket->flag = calloc(total, sizeof(int32_t));
    bucket->data = calloc(total, sizeof(struct set_t));

    bucket->index = 0;
    bucket->count = 0;
    bucket->total = total;
}

void free_bucket(struct bucket_t* bucket) {
    free(bucket->flag);
    free(bucket->data);
    free(bucket);
}

void expand_bucket(struct bucket_t* bucket) {
    int32_t count = bucket->total;
    int32_t total = count << 1;

    bucket->index = count;
    bucket->total = total;

    bucket->flag = realloc(bucket->flag, total * sizeof(int32_t));
    bucket->data = realloc(bucket->data, total * sizeof(struct set_t));

    memset(bucket->flag + count, 0, count * sizeof(int32_t));
}

int32_t acquire_slot(struct bucket_t* bucket) {
    if (++bucket->count == bucket->total)
        expand_bucket(bucket);

    int32_t index = bucket->index;

    do {
        index = (index + 1) % bucket->total;
    } while (bucket->flag[index]);

    ++bucket->flag[index];
    bucket->index = index;

    return bucket->index;
}

int32_t release_slot(struct bucket_t* bucket, int32_t index) {
    if (!bucket->flag[index])
        return 1;

    --bucket->flag[index];
    --bucket->count;

    return 0;
}
