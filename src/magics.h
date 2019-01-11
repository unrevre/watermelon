#ifndef MAGIC_H
#define MAGIC_H

/* search */

#define PLYSTART 1
#define PLYDELAY 2
#define PLYLIMIT 32

#define INFINITY 2048
#define INFDELAY (INFINITY - PLYDELAY)
#define INFSCORE (INFINITY + PLYSTART)

#define WSCORE (INFSCORE - 1)
#define LSCORE (INFSCORE)

/* transposition table */

#define HASHBITS 24
#define HASHSIZE (0x1 << HASHBITS)
#define HASHMASK (HASHSIZE - 0x1)

#define FEXACT 0x1
#define FLOWER 0x2
#define FUPPER 0x3

#define BASKETS 4

/* state */

#define RSIDE 0x0
#define BSIDE 0x8
#define EMPTY 0x7

#define PSBIT BSIDE
#define PSHIFT 0

#define b(i) (i >> 3)

#define o(s) (s ^ PSBIT)

#define p(p) (p & 0x7)
#define s(p) (p & PSBIT)

#define ps(s, p) ((p << PSHIFT) ^ s)
#define po(s, p) ((p << PSHIFT) ^ o(s))

#define empty EMPTY

#endif /* MAGIC_H */
