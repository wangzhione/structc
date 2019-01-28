#ifndef _RSQ_H
#define _RSQ_H

#include "msg.h"
#include "atom.h"

typedef struct rsq * rsq_t;

extern void rsq_delete(void);

extern rsq_t rsq_create(void);

extern int rsq_pop(rsq_t q, msg_t * psg);

extern int rsq_push(rsq_t q, const void * data, uint32_t sz);

#endif//_RSQ_H

