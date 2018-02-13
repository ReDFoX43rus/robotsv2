#ifndef CPTHREAD_DSEM_H
#define CPTHREAD_DSEM_H

#include "semaphore.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Dijkstra's semaphore */
typedef struct {
	int count;
	int limit;
	sem_t sem;
} dsem_t;

dsem_t *dsem_init(int count, int limit);
void dsem_p(dsem_t *dsem);
void dsem_v(dsem_t *dsem);
void dsem_destroy(dsem_t *dsem);
/* ================== */

#ifdef __cplusplus
}
#endif

#endif /* end of include guard: CPTHREAD_DSEM_H */
