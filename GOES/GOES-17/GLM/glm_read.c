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
#include <assert.h>
#include <netcdf.h>
#include "un_test.h"
#include "glm_data.h"

/* Attribute names. */
#define TITLE "title"
#define SUMMARY "summary"
#define PLATFORM_ID "platform_ID"

/* Number of timing runs when -t option is used. */
#define NUM_TRIALS 10

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

/* Read and unpack all the event data in the file. It will be loaded
 * into the pre-allocated array of struct event. */
int
read_event_vars(int ncid, int nevents, GLM_EVENT_T *event)
{
    /* Event varids. */
    int event_id_varid;
    int event_time_offset_varid, event_lat_varid, event_lon_varid;
    int event_energy_varid, event_parent_group_id_varid;

    /* Storage for packed data. */
    int *event_id = NULL;
    short *event_time_offset = NULL, *event_lat = NULL, *event_lon = NULL;
    short *event_energy = NULL;
    int *event_parent_group_id = NULL;

    /* Scale factors and offsets. */
    float event_time_offset_scale, event_time_offset_offset;
    float event_lat_scale, event_lat_offset;
    float event_lon_scale, event_lon_offset;
    float event_energy_scale, event_energy_offset;

    int i;
    int ret;

    /* Check inputs. */
    assert(ncid && event && nevents > 0);

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

    /* Find the varids for the event variables. Also get the scale
     * factors and offsets. */
    if ((ret = nc_inq_varid(ncid, EVENT_ID, &event_id_varid)))
	NC_ERR(ret);
    
    if ((ret = nc_inq_varid(ncid, EVENT_TIME_OFFSET, &event_time_offset_varid)))
	NC_ERR(ret);
    if ((ret = nc_get_att_float(ncid, event_time_offset_varid, SCALE_FACTOR, &event_time_offset_scale)))
	NC_ERR(ret);
    if ((ret = nc_get_att_float(ncid, event_time_offset_varid, ADD_OFFSET, &event_time_offset_offset)))
	NC_ERR(ret);
    
    if ((ret = nc_inq_varid(ncid, EVENT_LAT, &event_lat_varid)))
	NC_ERR(ret);
    if ((ret = nc_get_att_float(ncid, event_lat_varid, SCALE_FACTOR, &event_lat_scale)))
	NC_ERR(ret);
    if ((ret = nc_get_att_float(ncid, event_lat_varid, ADD_OFFSET, &event_lat_offset)))
	NC_ERR(ret);
    
    if ((ret = nc_inq_varid(ncid, EVENT_LON, &event_lon_varid)))
	NC_ERR(ret);
    if ((ret = nc_get_att_float(ncid, event_lon_varid, SCALE_FACTOR, &event_lon_scale)))
	NC_ERR(ret);
    if ((ret = nc_get_att_float(ncid, event_lon_varid, ADD_OFFSET, &event_lon_offset)))
	NC_ERR(ret);
    
    if ((ret = nc_inq_varid(ncid, EVENT_ENERGY, &event_energy_varid)))
	NC_ERR(ret);
    if ((ret = nc_get_att_float(ncid, event_energy_varid, SCALE_FACTOR, &event_energy_scale)))
	NC_ERR(ret);
    if ((ret = nc_get_att_float(ncid, event_energy_varid, ADD_OFFSET, &event_energy_offset)))
	NC_ERR(ret);

    /* event_parent_group_id is not packed. */
    if ((ret = nc_inq_varid(ncid, EVENT_PARENT_GROUP_ID, &event_parent_group_id_varid)))
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

    /* Unpack the data into our already-allocated array of struct
     * GLM_EVENT. */
    for (i = 0; i < nevents; i++)
    {
	event[i].id = event_id[i];
	event[i].time_offset = (float)event_time_offset[i]/event_time_offset_scale + event_time_offset_offset;
	event[i].lat = (float)event_lat[i]/event_lat_scale + event_lat_offset;
	event[i].lon = (float)event_lon[i]/event_lon_scale + event_lon_offset;
	event[i].energy = (float)event_energy[i]/event_energy_scale + event_energy_offset;
	event[i].parent_group_id = event_parent_group_id[i];
    }

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
  
    return 0;
}

/* Read and unpack all the group data in the file. It will be loaded
 * into the pre-allocated array of struct group. */
int
read_group_vars(int ncid, int ngroups, GLM_GROUP_T *group)
{
    return 0;
}

/* Read and unpack all the flash data in the file. It will be loaded
 * into the pre-allocated array of struct flash. */
int
read_flash_vars(int ncid, int nflashes, GLM_FLASH_T *flash)
{
    return 0;
}

/*
  From GOES R SERIESPRODUCT DEFINITION AND USERS’ GUIDE(PUG) Vol 3
  (https://www.goes-r.gov/users/docs/PUG-L1b-vol3.pdf)

  The classic model for netCDF (used by the GS) does not support
  unsigned integers larger than 8 bits.  Many of the variables in
  GOES-R netCDF files are unsigned integers of 16-bit or 32-bit
  length.  The following process is recommended to convert these
  unsigned integers:

  1.Retrieve the variable data from the netCDF file.

  2.For this variable, retrieve the attribute “_Unsigned”.

  3.If the “_Unsigned” attribute is set to “true” or “True”, then
  cast the variable data to be unsigned.

  The steps above must be completed before applying the
  scale_factor and add_offset values to convert from scaled
  integer to science units.  Also, the valid_range and _FillValue
  attribute values are to be governed by the “_Unsigned” attribute

  From a netCDF group email:
  https://www.unidata.ucar.edu/mailing_lists/archives/netcdfgroup/2002/msg00034.html

  Normally you store a group of numbers, all with the same scale
  and offset. So first you calculate the min and max of that group
  of numbers. Also let max_integer = maximum integer (eg for
  INTEGER*2 this equals 32,167).

  then
  offset = min
  scale = max_integer / (max - min)

  store this number into netcdf file:

  store_x = (x - offset) * scale = max_integer * (x - min) / (max - min)

  note that when x = min, store_x = 0, and when x = max, store_x
  max_integer.

  the reading program should use the formula

  x = store_x/scale + offset.

*/
int
glm_read_file(char *file_name, int verbose)
{
    int ncid;

    /* Dimensions and their lengths. */
    int event_dimid, group_dimid, flash_dimid;
    int number_of_time_bounds_dimid;
    int number_of_field_of_view_bounds_dimid;
    int number_of_wavelength_bounds_dimid;
    size_t nevents, ngroups, nflashes;
    size_t ntime_bounds, nfov_bounds, nwl_bounds;

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

    /* Flashes. Note that event_id and group_id are int, but flash_id
     * is short. */
    int flash_id_varid;
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

    /* Structs of events, groups, flashes. */
    GLM_EVENT_T *event;
    GLM_GROUP_T *group;
    GLM_FLASH_T *flash;

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

    if ((ret = nc_inq_dimid(ncid, NUMBER_OF_TIME_BOUNDS, &number_of_time_bounds_dimid)))
	NC_ERR(ret);
    if ((ret = nc_inq_dimlen(ncid, number_of_time_bounds_dimid, &ntime_bounds)))
	NC_ERR(ret);
    assert(ntime_bounds == EXTRA_DIM_LEN);

    if ((ret = nc_inq_dimid(ncid, NUMBER_OF_FIELD_OF_VIEW_BOUNDS, &number_of_field_of_view_bounds_dimid)))
	NC_ERR(ret);
    if ((ret = nc_inq_dimlen(ncid, number_of_field_of_view_bounds_dimid, &nfov_bounds)))
	NC_ERR(ret);
    assert(nfov_bounds == EXTRA_DIM_LEN);

    if ((ret = nc_inq_dimid(ncid, NUMBER_OF_WAVELENGTH_BOUNDS, &number_of_wavelength_bounds_dimid)))
	NC_ERR(ret);
    if ((ret = nc_inq_dimlen(ncid, number_of_wavelength_bounds_dimid, &nwl_bounds)))
	NC_ERR(ret);
    assert(nwl_bounds == EXTRA_DIM_LEN);

    if (verbose)
	printf("nflashes %d ngroups %d nevents %d\n", nflashes,
	       ngroups, nevents);

    /* Read the event vars. */
    if (!(event = malloc(nevents * sizeof(GLM_EVENT_T))))
	ERR;
    if ((ret = read_event_vars(ncid, nevents, event)))
	ERR;
    free(event);
    
    /* Read the event vars. */
    if (!(group = malloc(ngroups * sizeof(GLM_GROUP_T))))
	ERR;
    if ((ret = read_group_vars(ncid, ngroups, group)))
	ERR;
    free(group);
    
    /* Read the flash vars. */
    if (!(flash = malloc(nflashes * sizeof(GLM_FLASH_T))))
	ERR;
    if ((ret = read_flash_vars(ncid, nflashes, flash)))
	ERR;
    free(flash);
    
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

    /* Allocate storeage for flash variables. */
    if (!(flash_id = malloc(nflashes * sizeof(short))))
	ERR;
    if (!(flash_time_offset_of_first_event = malloc(nflashes * sizeof(short))))
	ERR;
    if (!(flash_time_offset_of_last_event = malloc(nflashes * sizeof(short))))
	ERR;
    if (!(flash_frame_time_offset_of_first_event = malloc(nflashes * sizeof(short))))
	ERR;
    if (!(flash_frame_time_offset_of_last_event = malloc(nflashes * sizeof(short))))
	ERR;
    if (!(flash_lat = malloc(nflashes * sizeof(float))))
	ERR;
    if (!(flash_lon = malloc(nflashes * sizeof(float))))
	ERR;
    if (!(flash_area = malloc(nflashes * sizeof(short))))
	ERR;
    if (!(flash_energy = malloc(nflashes * sizeof(short))))
	ERR;
    if (!(flash_quality_flag = malloc(nflashes * sizeof(short))))
	ERR;

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

    /* Find the varids for the flash variables. */
    if ((ret = nc_inq_varid(ncid, FLASH_ID, &flash_id_varid)))
	NC_ERR(ret);
    if ((ret = nc_inq_varid(ncid, FLASH_TIME_OFFSET_OF_FIRST_EVENT, &flash_time_offset_of_first_event_varid)))
	NC_ERR(ret);
    if ((ret = nc_inq_varid(ncid, FLASH_TIME_OFFSET_OF_LAST_EVENT, &flash_time_offset_of_last_event_varid)))
	NC_ERR(ret);
    if ((ret = nc_inq_varid(ncid, FLASH_FRAME_TIME_OFFSET_OF_FIRST_EVENT,
			    &flash_frame_time_offset_of_first_event_varid)))
	NC_ERR(ret);
    if ((ret = nc_inq_varid(ncid, FLASH_FRAME_TIME_OFFSET_OF_LAST_EVENT,
			    &flash_frame_time_offset_of_last_event_varid)))
	NC_ERR(ret);
    if ((ret = nc_inq_varid(ncid, FLASH_LAT, &flash_lat_varid)))
	NC_ERR(ret);
    if ((ret = nc_inq_varid(ncid, FLASH_LON, &flash_lon_varid)))
	NC_ERR(ret);
    if ((ret = nc_inq_varid(ncid, FLASH_AREA, &flash_area_varid)))
	NC_ERR(ret);
    if ((ret = nc_inq_varid(ncid, FLASH_ENERGY, &flash_energy_varid)))
	NC_ERR(ret);
    if ((ret = nc_inq_varid(ncid, FLASH_QUALITY_FLAG, &flash_quality_flag_varid)))
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

    /* Read the flash variables. */
    if ((ret = nc_get_var_short(ncid, flash_id_varid, flash_id)))
	NC_ERR(ret);
    if ((ret = nc_get_var_short(ncid, flash_time_offset_of_first_event_varid,
				flash_time_offset_of_first_event)))
	NC_ERR(ret);
    if ((ret = nc_get_var_short(ncid, flash_time_offset_of_last_event_varid,
				flash_time_offset_of_last_event)))
	NC_ERR(ret); 
    if ((ret = nc_get_var_short(ncid, flash_frame_time_offset_of_first_event_varid,
				flash_frame_time_offset_of_first_event)))
	NC_ERR(ret);
    if ((ret = nc_get_var_short(ncid, flash_frame_time_offset_of_last_event_varid,
				flash_frame_time_offset_of_last_event)))
	NC_ERR(ret);
    if ((ret = nc_get_var_float(ncid, flash_lat_varid, flash_lat)))
	NC_ERR(ret);
    if ((ret = nc_get_var_float(ncid, flash_lon_varid, flash_lon)))
	NC_ERR(ret);
    if ((ret = nc_get_var_short(ncid, flash_area_varid, flash_area)))
    	NC_ERR(ret);
    if ((ret = nc_get_var_short(ncid, flash_energy_varid, flash_energy)))
    	NC_ERR(ret);
    if ((ret = nc_get_var_short(ncid, flash_quality_flag_varid, flash_quality_flag)))
    	NC_ERR(ret);

    int product_time_varid;
    double product_time;
    int product_time_bounds_varid;
    double product_time_bounds[EXTRA_DIM_LEN];
    int lightning_wavelength_varid;
    float lightning_wavelength;
    int lightning_wavelength_bounds_varid;
    float lightning_wavelength_bounds[EXTRA_DIM_LEN];
    int group_time_threshold_varid;
    float group_time_threshold;
    int flash_time_threshold_varid;
    float flash_time_threshold;
    int lat_field_of_view_varid;
    float lat_field_of_view;
    int lat_field_of_view_bounds_varid;
    float lat_field_of_view_bounds[EXTRA_DIM_LEN];
    int goes_lat_lon_projection_varid;
    int goes_lat_lon_projection;
    int event_count_varid;
    int event_count;
    int group_count_varid;
    int group_count;
    int flash_count_varid;
    int flash_count;
    int percent_navigated_L1b_events_varid;
    float percent_navigated_L1b_events;
    int yaw_flip_flag_varid;
    signed char yaw_flip_flag;
    int nominal_satellite_subpoint_lat_varid;
    float nominal_satellite_subpoint_lat;
    int nominal_satellite_height_varid;
    float nominal_satellite_height;
    int nominal_satellite_subpoint_lon_varid;
    float nominal_satellite_subpoint_lon;
    int lon_field_of_view_varid;
    float lon_field_of_view;
    int lon_field_of_view_bounds_varid;
    float lon_field_of_view_bounds[EXTRA_DIM_LEN];
    int percent_uncorrectable_L0_errors_varid;
    float percent_uncorrectable_L0_errors;
    int algorithm_dynamic_input_data_container_varid;
    int algorithm_dynamic_input_data_container;
    int processing_parm_version_container_varid;
    int processing_parm_version_container;
    int algorithm_product_version_container_varid;
    int algorithm_product_version_container;

    /* Get varids and values of scalars and small vars. */
    if ((ret = nc_inq_varid(ncid, PRODUCT_TIME, &product_time_varid)))
	NC_ERR(ret);
    if ((ret = nc_get_var_double(ncid, product_time_varid, &product_time)))
    	NC_ERR(ret);

    if ((ret = nc_inq_varid(ncid, PRODUCT_TIME_BOUNDS, &product_time_bounds_varid)))
	NC_ERR(ret);
    if ((ret = nc_get_var_double(ncid, product_time_bounds_varid, product_time_bounds)))
    	NC_ERR(ret);
    
    if ((ret = nc_inq_varid(ncid, LIGHTNING_WAVELENGTH, &lightning_wavelength_varid)))
	NC_ERR(ret);
    if ((ret = nc_get_var_float(ncid, lightning_wavelength_varid, &lightning_wavelength)))
    	NC_ERR(ret);

    if ((ret = nc_inq_varid(ncid, LIGHTNING_WAVELENGTH_BOUNDS, &lightning_wavelength_bounds_varid)))
	NC_ERR(ret);
    if ((ret = nc_get_var_float(ncid, lightning_wavelength_bounds_varid, lightning_wavelength_bounds)))
    	NC_ERR(ret);
    
    if ((ret = nc_inq_varid(ncid, GROUP_TIME_THRESHOLD, &group_time_threshold_varid)))
	NC_ERR(ret);
    if ((ret = nc_get_var_float(ncid, group_time_threshold_varid, &group_time_threshold)))
    	NC_ERR(ret);

    if ((ret = nc_inq_varid(ncid, FLASH_TIME_THRESHOLD, &flash_time_threshold_varid)))
	NC_ERR(ret);
    if ((ret = nc_get_var_float(ncid, flash_time_threshold_varid, &flash_time_threshold)))
    	NC_ERR(ret);

    if ((ret = nc_inq_varid(ncid, LAT_FIELD_OF_VIEW, &lat_field_of_view_varid)))
	NC_ERR(ret);
    if ((ret = nc_get_var_float(ncid, lat_field_of_view_varid, &lat_field_of_view)))
    	NC_ERR(ret);

    if ((ret = nc_inq_varid(ncid, LAT_FIELD_OF_VIEW_BOUNDS, &lat_field_of_view_bounds_varid)))
	NC_ERR(ret);
    if ((ret = nc_get_var_float(ncid, lat_field_of_view_bounds_varid, lat_field_of_view_bounds)))
    	NC_ERR(ret);

    if ((ret = nc_inq_varid(ncid, GOES_LAT_LON_PROJECTION, &goes_lat_lon_projection_varid)))
	NC_ERR(ret);
    if ((ret = nc_get_var_int(ncid, goes_lat_lon_projection_varid, &goes_lat_lon_projection)))
    	NC_ERR(ret);

    if ((ret = nc_inq_varid(ncid, EVENT_COUNT, &event_count_varid)))
	NC_ERR(ret);
    if ((ret = nc_get_var_int(ncid, event_count_varid, &event_count)))
    	NC_ERR(ret);

    if ((ret = nc_inq_varid(ncid, GROUP_COUNT, &group_count_varid)))
	NC_ERR(ret);
    if ((ret = nc_get_var_int(ncid, group_count_varid, &group_count)))
    	NC_ERR(ret);

    if ((ret = nc_inq_varid(ncid, FLASH_COUNT, &flash_count_varid)))
	NC_ERR(ret);
    if ((ret = nc_get_var_int(ncid, flash_count_varid, &flash_count)))
    	NC_ERR(ret);

    if ((ret = nc_inq_varid(ncid, PERCENT_NAVIGATED_L1B_EVENTS, &percent_navigated_L1b_events_varid)))
	NC_ERR(ret);
    if ((ret = nc_get_var_float(ncid, percent_navigated_L1b_events_varid, &percent_navigated_L1b_events)))
    	NC_ERR(ret);

    if ((ret = nc_inq_varid(ncid, YAW_FLIP_FLAG, &yaw_flip_flag_varid)))
	NC_ERR(ret);
    if ((ret = nc_get_var_schar(ncid, yaw_flip_flag_varid, &yaw_flip_flag)))
    	NC_ERR(ret);

    if ((ret = nc_inq_varid(ncid, NOMINAL_SATELLITE_SUBPOINT_LAT, &nominal_satellite_subpoint_lat_varid)))
	NC_ERR(ret);
    if ((ret = nc_get_var_float(ncid, nominal_satellite_subpoint_lat_varid, &nominal_satellite_subpoint_lat)))
    	NC_ERR(ret);

    if ((ret = nc_inq_varid(ncid, NOMINAL_SATELLITE_HEIGHT, &nominal_satellite_height_varid)))
	NC_ERR(ret);
    if ((ret = nc_get_var_float(ncid, nominal_satellite_height_varid, &nominal_satellite_height)))
    	NC_ERR(ret);

    if ((ret = nc_inq_varid(ncid, NOMINAL_SATELLITE_SUBPOINT_LON, &nominal_satellite_subpoint_lon_varid)))
	NC_ERR(ret);
    if ((ret = nc_get_var_float(ncid, nominal_satellite_subpoint_lon_varid, &nominal_satellite_subpoint_lon)))
    	NC_ERR(ret);

    if ((ret = nc_inq_varid(ncid, LON_FIELD_OF_VIEW, &lon_field_of_view_varid)))
	NC_ERR(ret);
    if ((ret = nc_get_var_float(ncid, lon_field_of_view_varid, &lon_field_of_view)))
    	NC_ERR(ret);

    if ((ret = nc_inq_varid(ncid, LON_FIELD_OF_VIEW_BOUNDS, &lon_field_of_view_bounds_varid)))
	NC_ERR(ret);
    if ((ret = nc_get_var_float(ncid, lon_field_of_view_bounds_varid, lon_field_of_view_bounds)))
    	NC_ERR(ret);

    if ((ret = nc_inq_varid(ncid, PERCENT_UNCORRECTABLE_L0_ERRORS,
			    &percent_uncorrectable_L0_errors_varid)))
	NC_ERR(ret);
    if ((ret = nc_get_var_float(ncid, percent_uncorrectable_L0_errors_varid,
				&percent_uncorrectable_L0_errors)))
    	NC_ERR(ret);

    if ((ret = nc_inq_varid(ncid, ALGORITHM_DYNAMIC_INPUT_DATA_CONTAINER,
			    &algorithm_dynamic_input_data_container_varid)))
	NC_ERR(ret);
    if ((ret = nc_get_var_int(ncid, algorithm_dynamic_input_data_container_varid,
			      &algorithm_dynamic_input_data_container)))
    	NC_ERR(ret);

    if ((ret = nc_inq_varid(ncid, PROCESSING_PARM_VERSION_CONTAINER,
			    &processing_parm_version_container_varid)))
	NC_ERR(ret);
    if ((ret = nc_get_var_int(ncid, processing_parm_version_container_varid,
			      &processing_parm_version_container)))
    	NC_ERR(ret);

    if ((ret = nc_inq_varid(ncid, PROCESSING_PARM_VERSION_CONTAINER,
			    &processing_parm_version_container_varid)))
	NC_ERR(ret);
    if ((ret = nc_get_var_int(ncid, processing_parm_version_container_varid,
			      &processing_parm_version_container)))
    	NC_ERR(ret);

    if ((ret = nc_inq_varid(ncid, ALGORITHM_PRODUCT_VERSION_CONTAINER,
			    &algorithm_product_version_container_varid)))
	NC_ERR(ret);
    if ((ret = nc_get_var_int(ncid, algorithm_product_version_container_varid,
			      &algorithm_product_version_container)))
    	NC_ERR(ret);


    /* Close the data file. */
    if ((ret = nc_close(ncid)))
	NC_ERR(ret);

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

    /* Free flash storage. */
    if (flash_id)
	free(flash_id);
    if (flash_time_offset_of_first_event)
	free(flash_time_offset_of_first_event);
    if (flash_time_offset_of_last_event)
	free(flash_time_offset_of_last_event);
    if (flash_frame_time_offset_of_first_event)
	free(flash_frame_time_offset_of_first_event);
    if (flash_frame_time_offset_of_last_event)
	free(flash_frame_time_offset_of_last_event);
    if (flash_lat)
	free(flash_lat);
    if (flash_lon)
	free(flash_lon);
    if (flash_area)
	free(flash_area);
    if (flash_energy)
	free(flash_energy);
    if (flash_quality_flag)
	free(flash_quality_flag);

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
