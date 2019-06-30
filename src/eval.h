#ifndef EVAL_H
#define EVAL_H

#include <stdint.h>

union move_t;
struct transient_t;

/*!
 * eval
 * @ evaluate position
 */

int32_t eval(struct transient_t* state, int32_t side);

/*!
 * gain
 * @ evaluate material gain of a capture move
 */

int32_t gain(union move_t move);

#endif /* EVAL_H */
