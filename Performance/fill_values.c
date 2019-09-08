/* This program is to demonstrate some aspects of netCDF fill
 * values.
 *
 * Ed Hartnett, 9/8/19
 */
#include <stdio.h>
#include <time.h>
#include <sys/time.h> /* Extra high precision time info. */
#include <netcdf.h>

#define MILLION 1000000
#define NDIM2 2
#define DIM1_NAME "dim1"
#define DIM2_NAME "dim2"
#define DIM1_LEN 10000
#define DIM2_LEN 10000
#define VAR_NAME "var"

/* This macro prints an error message with line number and name of
 * test program. */
#define NCERR(ret) do {						  \
fflush(stdout); /* Make sure our stdout is synced with stderr. */ \
fprintf(stderr, "Sorry! Unexpected result, %s, line: %d %s\n", \
	__FILE__, __LINE__, nc_strerror(ret));                 \
return ret; \
} while (0)

/* This macro prints an error message with line number and name of
 * test program. */
#define ERR do {						  \
fflush(stdout); /* Make sure our stdout is synced with stderr. */ \
fprintf(stderr, "Sorry! Unexpected result, %s, line: %d\n", \
	__FILE__, __LINE__);                 \
return 99; \
} while (0)


#define FILE_NAME "fill_values.nc"
#define NUM_FILL_VALUE_MODES 2

/* Prototype. */
int nc4_timeval_subtract(struct timeval *result, struct timeval *x,
                         struct timeval *y);

int
main()
{
    int mode = 0;
    int ncid;
    int dimid[NDIM2], varid;
    int f;
    int ret;

    printf("fill\t time\n");
    for (f = 0; f < NUM_FILL_VALUE_MODES; f++)
    {
        struct timeval start_time, end_time, diff_time;
        int my_time_us;
        float my_time_sec;

        gettimeofday(&start_time, NULL);

        if ((ret = nc_create(FILE_NAME, mode, &ncid)))
            NCERR(ret);
        if (!f)
            if ((ret = nc_set_fill(ncid, NC_NOFILL, NULL)))
                NCERR(ret);
        if ((ret = nc_def_dim(ncid, DIM1_NAME, DIM1_LEN, &dimid[0])))
            NCERR(ret);
        if ((ret = nc_def_dim(ncid, DIM2_NAME, DIM2_LEN, &dimid[1])))
            NCERR(ret);
        if ((ret = nc_def_var(ncid, VAR_NAME, NC_INT, NDIM2, dimid, &varid)))
            NCERR(ret);
        if ((ret = nc_close(ncid)))
            NCERR(ret);

        gettimeofday(&end_time, NULL);
        if (nc4_timeval_subtract(&diff_time, &end_time, &start_time)) ERR;
        my_time_us = (int)diff_time.tv_sec * MILLION + (int)diff_time.tv_usec;
        my_time_sec = my_time_us / (float)MILLION;
        printf("%d \t %g\n", f, my_time_sec);
    }
    return 0;
}


/** Subtract the `struct timeval' values X and Y, storing the result in
   RESULT.  Return 1 if the difference is negative, otherwise 0.  This
   function from the GNU documentation. */
int
nc4_timeval_subtract (result, x, y)
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
