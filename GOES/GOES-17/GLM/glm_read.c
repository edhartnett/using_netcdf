/*
  Program to read data from the GOES-17 Global Lightning Mapper.

  Ed Hartnett, 10/10/19
  Amsterdam
*/

#define GLM_DATA_FILE "OR_GLM-L2-LCFA_G17_s20192692359400_e20192700000000_c20192700000028.nc"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netcdf.h>
#include "un_test.h"

/* Attribute names. */
#define TITLE "title"

#define USAGE   "\
  [-v]        Verbose\n\
  [-h]        Print output header\n"

static void
usage(void)
{
   fprintf(stderr, "glm_read -v -h\n%s", USAGE);
}

int
main(int argc, char **argv)
{
    int ncid;
    char *title;
    size_t att_len;
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
    if ((ret = nc_inq_attlen(ncid, NC_GLOBAL, TITLE, &att_len)))
	NC_ERR(ret);

    if (!(title = malloc(att_len * sizeof(char))))
	ERR;
    if ((ret = nc_get_att_text(ncid, NC_GLOBAL, TITLE, title)))
	NC_ERR(ret);
    if (verbose)
	printf("Title: %s\n", title);
    free(title);

    /* Close the data file. */
    if ((ret = nc_close(ncid)))
	NC_ERR(ret);

    if (verbose)
	printf("SUCCESS!\n");
    return 0;
}
