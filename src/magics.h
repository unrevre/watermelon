#ifndef MAGIC_H
#define MAGIC_H

/* search */

#define PLYSTART 0
#define PLYDELAY 2
#define PLYLIMIT 0x20

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

/* history */

#define STEPLIMIT 0x0100

#define AGEMASK 0x3

/* state */

#define RSIDE 0x0
#define BSIDE 0x1
#define EMPTY 0xe

#define PSIDEB 0x1
#define PSHIFT 1

#define o(s) (!s)

#define p(p) (p >> 1)
#define s(p) (p & PSIDEB)

#define ps(s, p) ((p << PSHIFT) + s)
#define po(s, p) ((p << PSHIFT) ^ o(s))

#define red RSIDE
#define black BSIDE
#define empty EMPTY

#define pass PSIDEB

#endif /* MAGIC_H */
