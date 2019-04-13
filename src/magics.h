#ifndef MAGIC_H
#define MAGIC_H

/* search */

#define PLYLIMIT    32

#define INFINITY    2048
#define INFPLUS     (INFINITY - 1)
#define INFMINUS    (-INFINITY)
#define INFLIMIT    (INFPLUS - PLYLIMIT)

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
