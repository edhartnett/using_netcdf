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

#define USAGE   "\
  [-v]        Verbose\n\
  [-h]        Print output header\n"

static void
usage(void)
{
   fprintf(stderr, "glm_read -v -h\n%s", USAGE);
}

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
main(int argc, char **argv)
{
    int ncid;
    int c;
    int verbose = 0;
    int ret;
    
    while ((c = getopt(argc, argv, "v")) != EOF)
	switch(c)
	{
	case 'v':
	    verbose++;
	    break;
	case '?':
	    usage();
	    return 1;
	}

    if (verbose)
	printf("Reading Geostationary Lightning Mapper data\n");

    /* Open the data file as read-only. */
    if ((ret = nc_open(GLM_DATA_FILE, NC_NOWRITE, &ncid)))
	NC_ERR(ret);

    /* Read some of the global attributes. The GLM data files comply
     * with the CF Conventions, and other metadata standards. */
    if (show_att(ncid, NC_GLOBAL, TITLE))
	ERR;
    if (show_att(ncid, NC_GLOBAL, PLATFORM_ID))
	ERR;
    if (show_att(ncid, NC_GLOBAL, SUMMARY))
	ERR;

    /* Close the data file. */
    if ((ret = nc_close(ncid)))
	NC_ERR(ret);

    if (verbose)
	printf("SUCCESS!\n");
    return 0;
}
