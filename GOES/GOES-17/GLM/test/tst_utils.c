/* Utility functions for tests.
 *
 * Ed Hartnett
 * 10/15/19
 * Amsterdam
 */

#include <time.h>
#include <sys/time.h>
#include "un_test.h"

/** Subtract the `struct timeval' values X and Y, storing the result in
    RESULT.  Return 1 if the difference is negative, otherwise 0.  This
    function from the GNU documentation. */
int
un_timeval_subtract (result, x, y)
    struct timeval *result, *x, *y;
{
    /* Perform the carry for the later subtraction by updating Y. */
    if (x->tv_usec < y->tv_usec) {
        int nsec = (y->tv_usec - x->tv_usec) / MILLION + 1;
        y->tv_usec -= MILLION * nsec;
        y->tv_sec += nsec;
    }
    if (x->tv_usec - y->tv_usec > MILLION) {
        int nsec = (x->tv_usec - y->tv_usec) / MILLION;
        y->tv_usec += MILLION * nsec;
        y->tv_sec -= nsec;
    }

    /* Compute the time remaining to wait.
       `tv_usec' is certainly positive. */
    result->tv_sec = x->tv_sec - y->tv_sec;
    result->tv_usec = x->tv_usec - y->tv_usec;

    /* Return 1 if result is negative. */
    return x->tv_sec < y->tv_sec;
}
