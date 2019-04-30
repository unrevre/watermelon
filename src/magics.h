#ifndef MAGIC_H
#define MAGIC_H

/* structs */

#define BITS        128
#define PIECES      15

#define FILES       9
#define RANKS       10

#define WIDTH       (FILES)
#define HEIGHT      (RANKS)

#define POINTS      (WIDTH * HEIGHT)

/* shifts */

#define _1n(a)      (a << (WIDTH))
#define _1e(a)      (a << 1)
#define _1s(a)      (a >> (WIDTH))
#define _1w(a)      (a >> 1)

#define _1f(a, s)   ((a << (WIDTH)) >> (s ? (WIDTH << 1) : 0))

#define _1n1e(a)    _1n(_1e(a))
#define _1n1w(a)    _1n(_1w(a))
#define _1s1e(a)    _1s(_1e(a))
#define _1s1w(a)    _1s(_1w(a))

#define _2n(a)      _1n(_1n(a))
#define _2e(a)      _1e(_1e(a))
#define _2s(a)      _1s(_1s(a))
#define _2w(a)      _1w(_1w(a))

#define _2n1e(a)    _2n(_1e(a))
#define _2n1w(a)    _2n(_1w(a))
#define _2e1n(a)    _2e(_1n(a))
#define _2e1s(a)    _2e(_1s(a))
#define _2s1e(a)    _2s(_1e(a))
#define _2s1w(a)    _2s(_1w(a))
#define _2w1n(a)    _2w(_1n(a))
#define _2w1s(a)    _2w(_1s(a))

#define _2n2e(a)    _2n(_2e(a))
#define _2n2w(a)    _2n(_2w(a))
#define _2s2e(a)    _2s(_2e(a))
#define _2s2w(a)    _2s(_2w(a))

/* search */

#define PLYLIMIT    32

#define INFINITY    2048
#define INFPLUS     (INFINITY - 1)
#define INFMINUS    (-INFINITY)
#define INFLIMIT    (INFPLUS - PLYLIMIT)

#define DRAW        0

/* state */

#define red     0x0
#define black   0x1
#define pass    black
#define empty   0xe

#define shiftp  1

#define o(s) (!s)

#define p(p) (p >> shiftp)
#define s(p) (p & black)

#define ps(s, p) ((p << shiftp) + s)
#define po(s, p) ((p << shiftp) ^ o(s))

#endif /* MAGIC_H */
