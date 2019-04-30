#ifndef MASKS_H
#define MASKS_H

#include "magics.h"

extern __uint128_t BMASK;

extern __uint128_t PMASK[BITS];

extern __uint128_t UMASK[POINTS];
extern __uint128_t LMASK[POINTS];

extern __uint128_t RMASK[POINTS];
extern __uint128_t FMASK[POINTS];

extern __uint128_t OMASK[POINTS];

extern __uint128_t JMASK[2];
extern __uint128_t SMASK[2];
extern __uint128_t XMASK[2];
extern __uint128_t ZMASK[2];

extern __uint128_t FMASKN0;
extern __uint128_t FMASKN8;
extern __uint128_t FMASKN01;
extern __uint128_t FMASKN78;

/*!
 * init_masks
 * @ initialise masks
 */

void init_masks(void);

#endif /* MASKS_H */
