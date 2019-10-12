/*
  Program to read data from the GOES-17 Global Lightning Mapper.
  
  Ed Hartnett, 10/10/19
  Amsterdam
*/

#define GLM_DATA_FILE "OR_GLM-L2-LCFA_G17_s20192692359400_e20192700000000_c20192700000028.nc"

#include <stdio.h>
#include <stdlib.h>
#include <netcdf.h>

/* Attribute names. */
#define TITLE "title"

/* This macro prints an error message with line number and name of
 * test program. */
#define ERR do {                                                                \
        fflush(stdout); /* Make sure our stdout is synced with stderr. */       \
        fprintf(stderr, "Sorry! Unexpected result, %s, line: %d\n",             \
                __FILE__, __LINE__);                                            \
        fflush(stderr);                                                         \
        return 2;                                                               \
    } while (0)

/* This macro prints an error message with line number and name of
 * test program, and the netCDF error string. */
#define NC_ERR(stat) do {                                                       \
        fflush(stdout); /* Make sure our stdout is synced with stderr. */       \
        fprintf(stderr, "Sorry! Unexpected result, %s, line: %d %s\n",          \
                __FILE__, __LINE__, nc_strerror(stat));                         \
        fflush(stderr);                                                         \
        return 2;                                                               \
    } while (0)

int main()
{
    int ncid;
    char *title;
    size_t att_len;
    int ret;
    
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
    printf("Title: %s\n", title);
    free(title);
    
    /* Close the data file. */
    if ((ret = nc_close(ncid)))
	NC_ERR(ret);

    printf("SUCCESS!\n");
    return 0;
}
