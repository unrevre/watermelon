#ifndef MAGIC_H
#define MAGIC_H

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

/* shifts */

#define s1n(a)      (a << (WIDTH))
#define s1e(a)      (a << 1)
#define s1s(a)      (a >> (WIDTH))
#define s1w(a)      (a >> 1)

#define s1f(a, s)   ((a << (WIDTH)) >> (s ? (WIDTH << 1) : 0))

#define s1n1e(a)    s1n(s1e(a))
#define s1n1w(a)    (a << (WIDTH - 1))
#define s1s1e(a)    (a >> (WIDTH - 1))
#define s1s1w(a)    s1s(s1w(a))

#define s2n(a)      s1n(s1n(a))
#define s2e(a)      s1e(s1e(a))
#define s2s(a)      s1s(s1s(a))
#define s2w(a)      s1w(s1w(a))

#define s2n1e(a)    s2n(s1e(a))
#define s2n1w(a)    s1n(s1n1w(a))
#define s2e1n(a)    s2e(s1n(a))
#define s2e1s(a)    (a >> (WIDTH - 2))
#define s2s1e(a)    s1s(s1s1e(a))
#define s2s1w(a)    s2s(s1w(a))
#define s2w1n(a)    (a << (WIDTH - 2))
#define s2w1s(a)    s2w(s1s(a))

#define s2n2e(a)    s2n(s2e(a))
#define s2n2w(a)    s1n1w(s1n1w(a))
#define s2s2e(a)    s1s1e(s1s1e(a))
#define s2s2w(a)    s2s(s2w(a))

/* offsets */

#define i1n(i)      (PMASK[i + WIDTH])
#define i1e(i)      (PMASK[i + 1])
#define i1s(i)      (PMASK[i - WIDTH])
#define i1w(i)      (PMASK[i - 1])

#define i1n1e(i)    (PMASK[i + WIDTH + 1])
#define i1n1w(i)    (PMASK[i + WIDTH - 1])
#define i1s1e(i)    (PMASK[i - WIDTH + 1])
#define i1s1w(i)    (PMASK[i - WIDTH - 1])

#define i2n1e(i)    (PMASK[i + (WIDTH << 1) + 1])
#define i2n1w(i)    (PMASK[i + (WIDTH << 1) - 1])
#define i2e1n(i)    (PMASK[i + WIDTH + 2])
#define i2e1s(i)    (PMASK[i - WIDTH + 2])
#define i2w1n(i)    (PMASK[i + WIDTH - 2])

#define i2n2e(i)    (PMASK[i + (WIDTH << 1) + 2])
#define i2n2w(i)    (PMASK[i + (WIDTH << 1) - 2])

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

#endif /* MAGIC_H */
