#ifndef NMEA_GPS_PARSER_H
#define NMEA_GPS_PARSER_H

#include <stdint.h>

typedef enum {
    SS,
    SG0,
    SN,
    SG1,
    SG2,
    SA,
    CC,
    UTC,
    LAT,
    LATC,
    LON,
    LONC
} nmea_states_t;

/**
 * @brief Parsed latitude and longitude data from a GNGGA sentence.
 *
 * Latitude and longitude are stored as scaled integer values. For example,
 * `2043.70259` is stored as `204370259`.
 */
typedef struct {
    uint32_t latitude;
    uint32_t longitude;
    char latitudeDirection;
    char longitudeDirection;
    volatile uint8_t valid;
} gps_data_t;

void GPS_parser_init(gps_data_t *);
void GPS_parser(gps_data_t *, uint8_t);
void GPS_PrintData(gps_data_t *);
void GPS_ProcessRx(gps_data_t *);

#endif /* NMEA_GPS_PARSER_H */
