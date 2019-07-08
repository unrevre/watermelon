#ifndef EVAL_H
#define EVAL_H

#include "structs.h"

#include <stdint.h>

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
