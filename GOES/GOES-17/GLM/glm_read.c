/*
  Program to read data from the GOES-17 Global Lightning Mapper.

  Ed Hartnett, 10/10/19
  Amsterdam
*/

#define GLM_DATA_FILE "OR_GLM-L2-LCFA_G17_s20192692359400_e20192700000000_c20192700000028.nc"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h> /* Extra high precision time info. */
#include <math.h>
#include <netcdf.h>
#include "un_test.h"

/* Attribute names. */
#define TITLE "title"
#define SUMMARY "summary"
#define PLATFORM_ID "platform_ID"

/* Number of timing runs. */
#define NUM_TRIALS 10

/* These are dimension names in the GLM data file. */
#define NUMBER_OF_FLASHES "number_of_flashes"
#define NUMBER_OF_GROUPS "number_of_groups"
#define NUMBER_OF_EVENTS "number_of_events"

/* Usage description. */
#define USAGE   "\
  [-v]        Verbose\n\
  [-t]        Perform timing runs\n"

/* Read a text attribute and print its value. */
int
show_att(int ncid, int varid, char *name)
{
    char *value;
    size_t att_len;
    int ret;

    /* How long is it? */
    if ((ret = nc_inq_attlen(ncid, varid, name, &att_len)))
	NC_ERR(ret);

    /* Allocate storage. Add 1 for the C null terminator. */
    if (!(value = malloc((att_len + 1) * sizeof(char))))
	ERR;

    /* Read the attribute value. */
    if ((ret = nc_get_att_text(ncid, varid, name, value)))
	NC_ERR(ret);

    /* Add null terminator for C. */
    value[att_len] = 0;

    /* Print it. */
    printf("%s: %s\n", name, value);

    /* Free resources. */
    free(value);
    return 0;
}

int
glm_read_file(char *file_name, int verbose)
{
    int ncid;
    size_t nevents, ngroups, nflashes;
    int event_dimid, group_dimid, flash_dimid;
    int ret;
    
    /* Open the data file as read-only. */
    if ((ret = nc_open(file_name, NC_NOWRITE, &ncid)))
	NC_ERR(ret);

    /* Optionally display some of the global attributes. The GLM data
     * files comply with the CF Conventions, and other metadata
     * standards. */
    if (verbose)
    {
	if (show_att(ncid, NC_GLOBAL, TITLE))
	    ERR;
	if (show_att(ncid, NC_GLOBAL, PLATFORM_ID))
	    ERR;
	if (show_att(ncid, NC_GLOBAL, SUMMARY))
	    ERR;
    }

    /* Read the size of the dimensions. */
    if ((ret = nc_inq_dimid(ncid, NUMBER_OF_FLASHES, &flash_dimid)))
	NC_ERR(ret);
    if ((ret = nc_inq_dimlen(ncid, flash_dimid, &nflashes)))
	NC_ERR(ret);
    
    if ((ret = nc_inq_dimid(ncid, NUMBER_OF_GROUPS, &group_dimid)))
	NC_ERR(ret);
    if ((ret = nc_inq_dimlen(ncid, group_dimid, &ngroups)))
	NC_ERR(ret);

    if ((ret = nc_inq_dimid(ncid, NUMBER_OF_EVENTS, &event_dimid)))
	NC_ERR(ret);
    if ((ret = nc_inq_dimlen(ncid, event_dimid, &nevents)))
	NC_ERR(ret);

    /* Close the data file. */
    if ((ret = nc_close(ncid)))
	NC_ERR(ret);
    
    return 0;
}

int
main(int argc, char **argv)
{
    int c;
    int verbose = 0;
    int timing = 0;
    struct timeval start_time, end_time, diff_time;
    int meta_read_us = 0, meta_max_read_us = 0, meta_min_read_us = NC_MAX_INT;
    int meta_tot_read_us = 0, meta_avg_read_us;
    int num_trials = 1;
    int t;
    
    while ((c = getopt(argc, argv, "vt")) != EOF)
	switch(c)
	{
	case 'v':
	    verbose++;
	    break;
	case 't':
	    timing++;
	    num_trials = NUM_TRIALS;
	    break;
	case '?':
	    fprintf(stderr, "glm_read -v -h\n%s", USAGE);
	    return 1;
	}

    if (verbose)
	printf("Reading Geostationary Lightning Mapper data\n");

    for (t = 0; t < num_trials; t++)
    {
	/* Clear all buffers. */
	if (timing)
	    system("./clearcache.sh");

	/* Start timer. */
	if (gettimeofday(&start_time, NULL))
	    ERR;

	/* Read file. */
	if (glm_read_file(GLM_DATA_FILE, verbose))
	    ERR;

	/* Handle timing. */
	if (gettimeofday(&end_time, NULL)) ERR;
	if (un_timeval_subtract(&diff_time, &end_time, &start_time)) ERR;
	meta_read_us = (int)diff_time.tv_sec * MILLION + (int)diff_time.tv_usec;
	if (meta_read_us > meta_max_read_us)
	    meta_max_read_us = meta_read_us;
	if (meta_read_us < meta_min_read_us)
	    meta_min_read_us = meta_read_us;
	meta_tot_read_us += meta_read_us;
    }

    if (timing)
    {
	meta_avg_read_us = meta_tot_read_us / NUM_TRIALS;
	printf("meta_avg_read_us \t meta_min_read_us \t meta_max_read_us\n");
	printf("%d\t\t\t\t%d\t\t\t\t%d\n", meta_avg_read_us, meta_min_read_us,
	       meta_max_read_us);
    }
    
    if (verbose)
	printf("SUCCESS!\n");
    return 0;
}
