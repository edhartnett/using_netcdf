/* Header file for data from the Geostationary Lightning Mapper.
 *
 * Ed Hartnett
 * 10/27/19
 * Amsterdam
 */
#ifndef _UN_GLM_DATA_H
#define _UN_GLM_DATA_H

/* The three dimensions number_of_time_bounds,
 * number_of_field_of_view_bounds, number_of_wavelength_bounds have
 * a length of 2. */
#define EXTRA_DIM_LEN 2

/* These are dimension names in the GLM data file. */
#define NUMBER_OF_FLASHES "number_of_flashes"
#define NUMBER_OF_GROUPS "number_of_groups"
#define NUMBER_OF_EVENTS "number_of_events"
#define NUMBER_OF_TIME_BOUNDS "number_of_time_bounds"
#define NUMBER_OF_FIELD_OF_VIEW_BOUNDS "number_of_field_of_view_bounds"
#define NUMBER_OF_WAVELENGTH_BOUNDS "number_of_wavelength_bounds"

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
#define FLASH_LON "flash_lon"
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
#define GOES_LAT_LON_PROJECTION "goes_lat_lon_projection"
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

/* Attribute names. */
#define SCALE_FACTOR "scale_factor"
#define ADD_OFFSET "add_offset"

typedef struct GLM_EVENT
{
    int id;
    unsigned int time_offset;
    unsigned int lat;
    unsigned int lon;
    unsigned int energy;
    unsigned int parent_group_id;
} GLM_EVENT_T;

typedef struct GLM_GROUP
{
    int id;
    unsigned int time_offset;
    unsigned int lat;
    unsigned int lon;
    unsigned int energy;
    unsigned int parent_flash_id;
} GLM_GROUP_T;

typedef struct GLM_FLASH
{
    int id;
    unsigned int time_offset;
    unsigned int lat;
    unsigned int lon;
    unsigned int energy;
} GLM_FLASH_T;

#endif /* _UN_GLM_DATA_H */
