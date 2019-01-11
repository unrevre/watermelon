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

#endif /* MAGIC_H */
