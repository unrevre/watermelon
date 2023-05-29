#ifndef BUCKET_H
#define BUCKET_H

#include "structs.h"

#include <stdint.h>

struct bucket_t {
    int32_t* flag;
    void* data;

    int32_t index;
    int32_t count;
    int32_t total;
};

extern struct bucket_t bucket;

struct set_t {
    union move_t data[128];
};

void init_bucket(struct bucket_t* bucket, int32_t total);
void free_bucket(struct bucket_t* bucket);

int32_t acquire_slot(struct bucket_t* bucket);
int32_t release_slot(struct bucket_t* bucket, int32_t index);

#endif /* BUCKET_H */
