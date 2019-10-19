/*
  Program to read data from the GOES-17 Global Lightning Mapper.

  Ed Hartnett, 10/10/19
  Amsterdam
*/

#define GLM_DATA_FILE "OR_GLM-L2-LCFA_G17_s20192692359400_e20192700000000_c20192700000028.nc"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
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

/* Variable names. */
#define EVENT_ID "event_id"
#define EVENT_TIME_OFFSET "event_time_offset"
#define EVENT_LAT "event_lat"
#define EVENT_LON "event_lon"
#define EVENT_ENERGY "event_energy"
#define EVENT_PARENT_GROUP_ID "event_parent_group_id"
#define GROUP_ID "group_id"
#define GROUP_TIME_OFFSET "group_time_offset"
#define GROUP_FRAME_TIME_OFFSET "group_frame_time_offset"
#define GROUP_LAT "group_lat"
#define GROUP_LON "group_lon"
#define GROUP_AREA "group_area"
#define GROUP_ENERGY "group_energy"
#define GROUP_PARENT_FLASH_ID "group_parent_flash_id"
#define GROUP_QUALITY_FLAG "group_quality_flag"
#define FLASH_ID "flash_id"
#define FLASH_TIME_OFFSET_OF_FIRST_EVENT "flash_time_offset_of_first_event"
#define FLASH_TIME_OFFSET_OF_LAST_EVENT "flash_time_offset_of_last_event"
#define FLASH_FRAME_TIME_OFFSET_OF_FIRST_EVENT "flash_frame_time_offset_of_first_event"
#define FLASH_FRAME_TIME_OFFSET_OF_LAST_EVENT "flash_frame_time_offset_of_last_event"
#define FLASH_LAT "flash_lat"
#define FLAGS_LON "flash_lon"
#define FLASH_AREA "flash_area"
#define FLASH_ENERGY "flash_energy"
#define FLASH_QUALITY_FLAG "flash_quality_flag"
#define PRODUCT_TIME "product_time"
#define PRODUCT_TIME_BOUNDS "product_time_bounds"
#define LIGHTNING_WAVELENGTH "lightning_wavelength"
#define LIGHTNING_WAVELENGTH_BOUNDS "lightning_wavelength_bounds"
#define GROUP_TIME_THRESHOLD "group_time_threshold"
#define FLASH_TIME_THRESHOLD "flash_time_threshold"
#define LAT_FIELD_OF_VIEW "lat_field_of_view"
#define LAT_FIELD_OF_VIEW_BOUNDS "lat_field_of_view_bounds"
#define LAT_FIELD_OF_VIEW_BOUNDS "lat_field_of_view_bounds"
#define EVENT_COUNT "event_count"
#define GROUP_COUNT "group_count"
#define FLASH_COUNT "flash_count"
#define PERCENT_NAVIGATED_L1B_EVENTS "percent_navigated_L1b_events"
#define YAW_FLIP_FLAG "yaw_flip_flag"
#define NOMINAL_SATELLITE_SUBPOINT_LAT "nominal_satellite_subpoint_lat"
#define NOMINAL_SATELLITE_HEIGHT "nominal_satellite_height"
#define NOMINAL_SATELLITE_SUBPOINT_LON "nominal_satellite_subpoint_lon"
#define LON_FIELD_OF_VIEW "lon_field_of_view"
#define LON_FIELD_OF_VIEW_BOUNDS "lon_field_of_view_bounds"
#define PERCENT_UNCORRECTABLE_L0_ERRORS "percent_uncorrectable_L0_errors"
#define ALGORITHM_DYNAMIC_INPUT_DATA_CONTAINER "algorithm_dynamic_input_data_container"
#define PROCESSING_PARM_VERSION_CONTAINER "processing_parm_version_container"
#define ALGORITHM_PRODUCT_VERSION_CONTAINER "algorithm_product_version_container"

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

    /* Events. */
    int event_dimid, group_dimid, flash_dimid;
    int event_id_varid;
    int event_time_offset_varid, event_lat_varid, event_lon_varid;
    int event_energy_varid, event_parent_group_id_varid;
    int *event_id = NULL;
    short *event_time_offset = NULL, *event_lat = NULL, *event_lon = NULL;
    short *event_energy = NULL;
    int *event_parent_group_id = NULL; 

    /* Groups. */
    int group_id_varid, group_time_offset_varid;
    int group_frame_time_offset_varid, group_lat_varid, group_lon_varid;
    int group_area_varid, group_energy_varid, group_parent_flash_id_varid;
    int group_quality_flag_varid;
    int *group_id = NULL;
    short *group_time_offset = NULL;
    short *group_frame_time_offset = NULL;
    float *group_lat = NULL, *group_lon = NULL;
    short *group_area = NULL, *group_energy = NULL, *group_parent_flash_id = NULL;
    short *group_quality_flag = NULL;

    /* Flashes. */
    int flash_id_varid, flash_time_offset_varid;
    int flash_time_offset_of_first_event_varid;
    int flash_time_offset_of_last_event_varid;
    int flash_frame_time_offset_of_first_event_varid;
    int flash_frame_time_offset_of_last_event_varid;
    int flash_lat_varid, flash_lon_varid;
    int flash_area_varid, flash_energy_varid;
    int flash_quality_flag_varid;
    short *flash_id = NULL;
    short *flash_time_offset_of_first_event = NULL;
    short *flash_time_offset_of_last_event = NULL;
    short *flash_frame_time_offset_of_first_event = NULL;
    short *flash_frame_time_offset_of_last_event = NULL;
    float *flash_lat = NULL, *flash_lon = NULL;
    short *flash_area = NULL, *flash_energy = NULL;
    short *flash_quality_flag = NULL;

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
    if (verbose)
	printf("nflashes %d ngroups %d nevents %d\n", nflashes,
	       ngroups, nevents);

    /* Allocate storeage for event variables. */
    if (!(event_id = malloc(nevents * sizeof(int))))
	ERR;
    if (!(event_time_offset = malloc(nevents * sizeof(short))))
	ERR;
    if (!(event_lat = malloc(nevents * sizeof(short))))
	ERR;
    if (!(event_lon = malloc(nevents * sizeof(short))))
	ERR;
    if (!(event_energy = malloc(nevents * sizeof(short))))
	ERR;
    if (!(event_parent_group_id = malloc(nevents * sizeof(int))))
	ERR;

        /* Allocate storeage for group variables. */
    if (!(group_id = malloc(ngroups * sizeof(int))))
	ERR;
    if (!(group_time_offset = malloc(ngroups * sizeof(short))))
	ERR;
    if (!(group_frame_time_offset = malloc(ngroups * sizeof(short))))
	ERR;
    if (!(group_lat = malloc(ngroups * sizeof(float))))
	ERR;
    if (!(group_lon = malloc(ngroups * sizeof(float))))
	ERR;
    if (!(group_area = malloc(ngroups * sizeof(short))))
	ERR;
    if (!(group_energy = malloc(ngroups * sizeof(short))))
	ERR;
    if (!(group_parent_flash_id = malloc(ngroups * sizeof(short))))
	ERR;
    if (!(group_quality_flag = malloc(ngroups * sizeof(int))))
	ERR;

    /* Find the varids for the event variables. */
    if ((ret = nc_inq_varid(ncid, EVENT_ID, &event_id_varid)))
	NC_ERR(ret);
    if ((ret = nc_inq_varid(ncid, EVENT_TIME_OFFSET, &event_time_offset_varid)))
	NC_ERR(ret);
    if ((ret = nc_inq_varid(ncid, EVENT_LAT, &event_lat_varid)))
	NC_ERR(ret);
    if ((ret = nc_inq_varid(ncid, EVENT_LON, &event_lon_varid)))
	NC_ERR(ret);
    if ((ret = nc_inq_varid(ncid, EVENT_ENERGY, &event_energy_varid)))
	NC_ERR(ret);
    if ((ret = nc_inq_varid(ncid, EVENT_PARENT_GROUP_ID, &event_parent_group_id_varid)))
	NC_ERR(ret);
    
    /* Find the varids for the group variables. */
    if ((ret = nc_inq_varid(ncid, GROUP_ID, &group_id_varid)))
	NC_ERR(ret);
    if ((ret = nc_inq_varid(ncid, GROUP_TIME_OFFSET, &group_time_offset_varid)))
	NC_ERR(ret);
    if ((ret = nc_inq_varid(ncid, GROUP_FRAME_TIME_OFFSET, &group_frame_time_offset_varid)))
	NC_ERR(ret);
    if ((ret = nc_inq_varid(ncid, GROUP_LAT, &group_lat_varid)))
	NC_ERR(ret);
    if ((ret = nc_inq_varid(ncid, GROUP_LON, &group_lon_varid)))
	NC_ERR(ret);
    if ((ret = nc_inq_varid(ncid, GROUP_AREA, &group_area_varid)))
	NC_ERR(ret);
    if ((ret = nc_inq_varid(ncid, GROUP_ENERGY, &group_energy_varid)))
	NC_ERR(ret);
    if ((ret = nc_inq_varid(ncid, GROUP_PARENT_FLASH_ID, &group_parent_flash_id_varid)))
	NC_ERR(ret);
    if ((ret = nc_inq_varid(ncid, GROUP_QUALITY_FLAG, &group_quality_flag_varid)))
	NC_ERR(ret);
    
    /* Read the event variables. */
    if ((ret = nc_get_var_int(ncid, event_id_varid, event_id)))
	NC_ERR(ret);
    if ((ret = nc_get_var_short(ncid, event_time_offset_varid, event_time_offset)))
	NC_ERR(ret);
    if ((ret = nc_get_var_short(ncid, event_lat_varid, event_lat)))
	NC_ERR(ret);
    if ((ret = nc_get_var_short(ncid, event_lon_varid, event_lon)))
	NC_ERR(ret);
    if ((ret = nc_get_var_short(ncid, event_energy_varid, event_energy)))
    	NC_ERR(ret);
    if ((ret = nc_get_var_int(ncid, event_parent_group_id_varid, event_parent_group_id)))
    	NC_ERR(ret);

    /* Read the group variables. */
    if ((ret = nc_get_var_int(ncid, group_id_varid, group_id)))
	NC_ERR(ret);
    if ((ret = nc_get_var_short(ncid, group_time_offset_varid, group_time_offset)))
	NC_ERR(ret);
    if ((ret = nc_get_var_short(ncid, group_frame_time_offset_varid, group_frame_time_offset)))
	NC_ERR(ret);
    if ((ret = nc_get_var_float(ncid, group_lat_varid, group_lat)))
	NC_ERR(ret);
    if ((ret = nc_get_var_float(ncid, group_lon_varid, group_lon)))
	NC_ERR(ret);
    if ((ret = nc_get_var_short(ncid, group_area_varid, group_area)))
    	NC_ERR(ret);
    if ((ret = nc_get_var_short(ncid, group_energy_varid, group_energy)))
    	NC_ERR(ret);
    if ((ret = nc_get_var_short(ncid, group_parent_flash_id_varid, group_parent_flash_id)))
    	NC_ERR(ret);
    if ((ret = nc_get_var_short(ncid, group_quality_flag_varid, group_quality_flag)))
    	NC_ERR(ret);
    
    /* Close the data file. */
    if ((ret = nc_close(ncid)))
	NC_ERR(ret);

    /* Free event storage. */
    if (event_id)
	free(event_id);
    if (event_time_offset)
	free(event_time_offset);
    if (event_lat)
	free(event_lat);
    if (event_lon)
	free(event_lon);
    if (event_energy)
	free(event_energy);
    if (event_parent_group_id)
	free(event_parent_group_id);

    /* Free group storage. */
    if (group_id)
	free(group_id);
    if (group_time_offset)
	free(group_time_offset);
    if (group_frame_time_offset)
	free(group_frame_time_offset);
    if (group_lat)
	free(group_lat);
    if (group_lon)
	free(group_lon);
    if (group_area)
	free(group_area);
    if (group_energy)
	free(group_energy);
    if (group_parent_flash_id)
	free(group_parent_flash_id);
    if (group_quality_flag)
	free(group_quality_flag);

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
    char new_file[1024];
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
	{
	    char cmd[1024];
	    char base_name[1024];
	    size_t len;

	    len = strlen(GLM_DATA_FILE) - 3;
	    strncpy(base_name, GLM_DATA_FILE, len);
	    base_name[len] = 0;
	    sprintf(new_file, "%s_%d.nc", base_name, t);
	    sprintf(cmd, "cp %s %s", GLM_DATA_FILE, new_file);
	    system(cmd);
	}
	else
	    strcpy(new_file, GLM_DATA_FILE);
	
	/* Start timer. */
	if (gettimeofday(&start_time, NULL))
	    ERR;

	/* Read file. */
	if (glm_read_file(new_file, verbose))
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

	if (timing)
	{
	    char cmd[1024];
	    sprintf(cmd, "rm %s", new_file);
	    system(cmd);
	}
	
	/* if (verbose) */
	    printf("meta_read_us %d\n", meta_read_us);
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
