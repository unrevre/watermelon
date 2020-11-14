#ifndef MAGIC_H
#define MAGIC_H

#include <stdint.h>

/* structs */

#define PIECES      15
#define BITS        128

#define PAD         16
#define FILES       9
#define RANKS       10
#define SENTINEL    1
#define OFFSET      9

#define WIDTH       (FILES + (SENTINEL << 1))
#define HEIGHT      (RANKS)

#define PADDED      (BITS + PAD)
#define INDICES     (FILES * RANKS)
#define POINTS      (WIDTH * HEIGHT + (OFFSET << 1))

/* displacements */

#define d1r         (WIDTH)
#define d1c         (1)

#define d2r         (d1r << 1)

#define d1n         (d1r)
#define d1e         (d1c)
#define d1s         (-d1r)
#define d1w         (-d1c)

#define d1f(s)      (s ? d1s : d1n)

#define d1n1e       (d1n + d1e)
#define d1n1w       (d1n + d1w)
#define d1s1e       (d1s + d1e)
#define d1s1w       (d1s + d1w)

#define d2n         (d1n + d1n)
#define d2e         (d1e + d1e)
#define d2s         (d1s + d1s)
#define d2w         (d1w + d1w)

#define d1n2e       (d1n + d2e)
#define d1n2w       (d1n + d2w)
#define d1s2e       (d1s + d2e)
#define d1s2w       (d1s + d2w)

#define d2n1e       (d2n + d1e)
#define d2n1w       (d2n + d1w)
#define d2s1e       (d2s + d1e)
#define d2s1w       (d2s + d1w)

#define d2n2e       (d2n + d2e)
#define d2n2w       (d2n + d2w)
#define d2s2e       (d2s + d2e)
#define d2s2w       (d2s + d2w)

/* shifts */

#define s1n(a)      (a << d1n)
#define s1e(a)      (a << d1e)
#define s1s(a)      (a >> d1n)
#define s1w(a)      (a >> d1e)

#define s1f(a, s)   ((a << d1r) >> (s ? d2r : 0))

#define s1n1e(a)    (a << d1n1e)
#define s1n1w(a)    (a << d1n1w)
#define s1s1e(a)    (a >> d1n1w)
#define s1s1w(a)    (a >> d1n1e)

#define s2n(a)      (a << d2n)
#define s2e(a)      (a << d2e)
#define s2s(a)      (a >> d2n)
#define s2w(a)      (a >> d2e)

#define s1n2e(a)    (a << d1n2e)
#define s1n2w(a)    (a << d1n2w)
#define s1s2e(a)    (a >> d1n2w)
#define s1s2w(a)    (a >> d1n2e)

#define s2n1e(a)    (a << d2n1e)
#define s2n1w(a)    (a << d2n1w)
#define s2s1e(a)    (a >> d2n1w)
#define s2s1w(a)    (a >> d2n1e)

#define s2n2e(a)    (a << d2n2e)
#define s2n2w(a)    (a << d2n2w)
#define s2s2e(a)    (a >> d2n2w)
#define s2s2w(a)    (a >> d2n2e)

/* offsets */

#define i1n(i)      (PMASK[i + d1n])
#define i1e(i)      (PMASK[i + d1e])
#define i1s(i)      (PMASK[i + d1s])
#define i1w(i)      (PMASK[i + d1w])

#define i1n1e(i)    (PMASK[i + d1n1e])
#define i1n1w(i)    (PMASK[i + d1n1w])
#define i1s1e(i)    (PMASK[i + d1s1e])
#define i1s1w(i)    (PMASK[i + d1s1w])

#define i1n2e(i)    (PMASK[i + d1n2e])
#define i1n2w(i)    (PMASK[i + d1n2w])
#define i1s2e(i)    (PMASK[i + d1s2e])
#define i1s2w(i)    (PMASK[i + d1s2w])

#define i2n1e(i)    (PMASK[i + d2n1e])
#define i2n1w(i)    (PMASK[i + d2n1w])
#define i2s1e(i)    (PMASK[i + d2s1e])
#define i2s1w(i)    (PMASK[i + d2s1w])

#define i2n2e(i)    (PMASK[i + d2n2e])
#define i2n2w(i)    (PMASK[i + d2n2w])
#define i2s2e(i)    (PMASK[i + d2s2e])
#define i2s2w(i)    (PMASK[i + d2s2w])

/* search */

#define PLYLIMIT    32

#define INFINITY    2048
#define INFPLUS     (INFINITY - 1)
#define INFMINUS    (-INFINITY)
#define INFLIMIT    (INFPLUS - PLYLIMIT)

#define DRAW        0

/* state */

#define red         0x0
#define black       0x1
#define pass        0x1
#define empty       0xe

#define o(s)        (!s)

#define p(p)        (p >> 1)
#define s(p)        (p & 0x1)

#define ps(s, p)    ((p << 1) + s)
#define po(s, p)    ((p << 1) ^ o(s))

/* history */

#define STEPLIMIT 0x100

/*!
 * to_external
 * @ convert internal index for output
 */

static __inline__ int32_t to_external(int32_t index) {
   int32_t y = (index - OFFSET) / WIDTH;
   int32_t x = (index - OFFSET) % WIDTH - SENTINEL;
   return y * FILES + x;
}

/*!
 * index_for
 * @ convert coordinates to internal index
 */

static __inline__ int32_t index_for(int32_t x, int32_t y) {
   return y * WIDTH + x + SENTINEL + OFFSET;
}

/*!
 * to_internal
 * @ convert external index to internal index
 */

static __inline__ int32_t to_internal(int32_t index) {
   return index_for(index % FILES, index / FILES);
}

#endif /* MAGIC_H */
