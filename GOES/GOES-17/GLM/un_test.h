/* Header file for test code as part of Using NetCDF.
 *
 * Ed Hartnett
 * 10/15/19
 * Amsterdam
 */
#ifndef _NC_TEST_H
#define _NC_TEST_H

/* This macro prints an error message with line number and name of
 * test program. */
#define ERR do {							\
        fflush(stdout); /* Make sure our stdout is synced with stderr. */ \
        fprintf(stderr, "Sorry! Unexpected result, %s, line: %d\n",	\
                __FILE__, __LINE__);					\
        fflush(stderr);							\
        return 2;							\
    } while (0)

/* This macro prints an error message with line number and name of
 * test program, and the netCDF error string. */
#define NC_ERR(stat) do {						\
        fflush(stdout); /* Make sure our stdout is synced with stderr. */ \
        fprintf(stderr, "Sorry! Unexpected result, %s, line: %d %s\n",	\
                __FILE__, __LINE__, nc_strerror(stat));			\
        fflush(stderr);							\
        return 2;							\
    } while (0)

#endif /* _NC_TEST_H */
