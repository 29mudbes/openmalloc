/* multithreaded fit openmalloc
 *
 * This module contains the function that implements the multithreaded malloc
 * strategy, malloc_mthread().
 *
 */

#include <limits.h>

/* openmalloc
 *
 * openmalloc returns the start address of the newly allocated memory.
 * It implements the Best fit algorithm, which tries to find the smallest free
 * block that is large enough.
 *
 */
/* number of bytes of memory to allocate */
void *malloc_mthread(size_t nbytes) {
    Header *p, *prevp;
    Header *moreroce(unsigned);
    unsigned nunits, min_size = INT_MAX;
    Header *minp = NULL, *minprevp = NULL;

    nunits = (nbytes + sizeof(Header) - 1) / sizeof(Header) + 1;
    if ((prevp = freep) == NULL) { /* no free list yet */
        base.s.ptr = freep = prevp = &base;
        base.s.size = 0;
    }
    pthread_mutex_lock(&base.s.lock);
    /*
     * Iterate over the free list and find the smallest block that is large
     * enough to hold nbytes of data.
     */

    for (p = prevp->s.ptr; ; prevp = p, p = p->s.ptr) {
        if (p->s.size >= nunits) { /* big enough */
            if (p->s.size == nunits) { /* exactly */
                prevp->s.ptr = p->s.ptr;
                freep = prevp;
                pthread_mutex_unlock(&base.s.lock);
                return (void *) (p + 1);
            }
            else {
                if (minp == NULL || p->s.size < min_size) {
                    minp = p;
                    minprevp = prevp;
                    min_size = minp->s.size;
                }
            }
        }
        if (p == freep) { /* wrapped around free list */
            if (minp != NULL) {
                /* allocate tail end */
                minp->s.size -= nunits;
                minp += minp->s.size;
                minp->s.size = nunits;
                freep = minprevp;
                pthread_mutex_unlock(&base.s.lock);
                return (void *) (minp + 1);
            }
            if ((p = morecore(nunits)) == NULL) {
                pthread_mutex_unlock(&base.s.lock);
                return NULL; /* none left */
            }
        }
    }
}