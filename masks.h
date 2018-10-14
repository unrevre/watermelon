#ifndef MASKS_H
#define MASKS_H

extern __uint128_t NMASK;
extern __uint128_t BMASK;

extern __uint128_t PMASK[90] __attribute__((aligned(64)));

extern __uint128_t UMASK[90] __attribute__((aligned(64)));
extern __uint128_t LMASK[90] __attribute__((aligned(64)));

extern __uint128_t RMASK[90] __attribute__((aligned(64)));
extern __uint128_t FMASK[90] __attribute__((aligned(64)));

extern __uint128_t JMASK[2];
extern __uint128_t SMASK[2];
extern __uint128_t XMASK[2];
extern __uint128_t ZMASK[2];

extern __uint128_t GMASK[2];

extern __uint128_t FMASKN0;
extern __uint128_t FMASKN8;
extern __uint128_t FMASKN01;
extern __uint128_t FMASKN78;

void init_masks(void);

#endif /* MASKS_H */
