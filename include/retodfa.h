#ifndef RETODFA_H
#define RETODFA_H

#include "tree.h"
#include "funcs.h"
#include "baseset.h"
#include "intset.h"
#include "chrset.h"
#include "basevector.h"
#include "intvector.h"


#define firstpos(h) pos(h,1)
#define lastpos(h)  pos(h,0)

struct _set {
    struct _hash *	hash;
};

int nullable(struct _node **);
/*returns int_set* */ base_set* pos(struct _node **,int);
/*returns int_set* */ base_set* followpos(base_vector**,struct _node **); /*base_vector* is an int_vector* */
char gcfprint(char c);

#endif
