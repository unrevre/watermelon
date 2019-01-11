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

#endif /* MAGIC_H */
