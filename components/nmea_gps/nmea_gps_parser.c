#include "nmea_gps_parser.h"
#include "../../hal/UART.h"

/**
 * @file nmea_gps_parser.c
 * @brief NMEA GNGGA parser and GPS data output implementation.
 *
 * The parser consumes sentences in the following format:
 * `$GNGGA,221056.000,2043.70259,N,10325.56150,W,1,11,1.1,1610.4,M,0.0,M,,*5E`.
 */

#define LL_POINT_POSITION 5U
#define LL_DEGREES_POSITION 8U

static nmea_states_t currentState;
static uint32_t factor;

/**
 * @brief Initialize the parser state and output values.
 *
 * @param gpsData GPS data structure to initialize.
 */
void GPS_parser_init(gps_data_t *gpsData)
{
    currentState = SS;
    factor = 0U;
    gpsData->latitude = 0U;
    gpsData->longitude = 0U;
    gpsData->latitudeDirection = 'I';
    gpsData->longitudeDirection = 'I';
    gpsData->valid = 0U;
}

/**
 * @brief Consume one character from a GNGGA sentence.
 *
 * @param gpsData GPS data structure updated by the state machine.
 * @param c Character received from the GPS module.
 */
void GPS_parser(gps_data_t *gpsData, uint8_t c)
{
    //$GPGGA,051319.000,2043.7067,N,10325.5443,W,6,04,2.4,1632.7,M,0.0,M,,*45
    //$GNGGA,221056.000,2043.70259,N,10325.56150,W,1,11,1.1,1610.4,M,0.0,M,,*5E
    switch(currentState){
            case SS:
                if(c == '$'){
                    currentState = SG0;
                }
                break;
            case SG0:
                if(c == 'G'){
                    currentState = SN;
                } else {
                    currentState = SS;
                }
                break;
            case SN:
                if(c == 'N'){
                    currentState = SG1;
                } else {
                    currentState = SS;
                }
                break;
            case SG1:
                if(c == 'G'){
                    currentState = SG2;
                } else {
                    currentState = SS;
                }
                break;
            case SG2:
                if(c == 'G'){
                    currentState = SA;
                } else {
                    currentState = SS;
                }
                break;
            case SA:
                if(c == 'A'){
                    currentState = CC;
                } else {
                    currentState = SS;
                }
                break;
            case CC:
                if(c == ','){
                    currentState = UTC;
                } else {
                    currentState = SS;
                }
                break;
            case UTC:
                if(c == ','){
                    factor = 100000000U;
                    currentState = LAT;
                } else if (c == '$'){
                    currentState = SS;
                } else {
                    currentState = UTC;
                }
                break;
            case LAT:
                if(c == ','){
                    currentState = LATC;
                } else if (c == '$'){
                    currentState = SS;
                } else {
                    if (c != '.' && factor > 0){
                        gpsData->latitude += ((uint8_t)(c - '0') * factor);
                        factor /= 10U;
                    }
                    currentState = LAT;
                }
                break;
            case LATC:
                if(c == ','){
                    factor = 1000000000U;
                    currentState = LON;
                } else if (c == '$'){
                    currentState = SS;
                } else {
                    gpsData->latitudeDirection = c;
                    currentState = LATC;
                }
                break;
            case LON:
                if(c == ','){
                    currentState = LONC;
                } else if (c == '$'){
                    currentState = SS;
                } else {
                    if (c != '.' && factor > 0){
                        gpsData->longitude += ((uint8_t)(c - '0') * factor);
                        factor /= 10U;
                    }
                    currentState = LON;
                }
                break;
            case LONC:
                if(c == ',' || c == '$'){
                    currentState = SS;
                    gpsData->valid = 1U;
                } else {
                    gpsData->longitudeDirection = c;
                    currentState = LONC;
                }
                break;
        }
}

    /**
     * @brief Drain the UART receive ring buffer into the GPS parser.
     *
     * @param gpsData GPS data structure updated with received sentence data.
     */
void GPS_ProcessRx(gps_data_t *gpsData)
{
    uint8_t c;
    while(ringBuffer_pop(&rxBuffer, &c)){
        GPS_parser(gpsData, c);
    }
}

/**
 * @brief Print an integer coordinate with degree and fractional markers.
 *
 * @param value Scaled coordinate value to print.
 * @param pointPosition Position at which to print the decimal point.
 * @param degreesPosition Position at which to print the degree marker.
 */
static void uprinti(uint32_t value, uint8_t pointPosition, uint8_t degreesPosition)
{
    char digits[13];
    uint8_t count = 0U;

    if (value == 0U) {
        uprintc('0');
        return;
    }
    digits[count++] = '\''; 
    while (value > 0U) {
        digits[count++] = (char)('0' + (value % 10U));
        value /= 10U;
        if (count == pointPosition+1U) { // +1 cuz \' takes one position
            digits[count++] = '.';
        }
        if (count == degreesPosition+1U) {
            digits[count++] = ' ';
        }
    }

    while (count > 0U) {
        uprintc(digits[--count]);
    }
}

/**
 * @brief Print valid parsed latitude and longitude values.
 *
 * The data is cleared after printing so the same sentence is not printed
 * again on the next processing loop.
 *
 * @param gpsData Parsed GPS data to print.
 */
void GPS_PrintData(gps_data_t *gpsData)
{
    if (0U == gpsData->valid) {
        return;
    }

    uprint("\r\n");
    uprint("Latitude: ");
    uprinti(gpsData->latitude, LL_POINT_POSITION, LL_DEGREES_POSITION);
    uprintc(gpsData->latitudeDirection);
    uprint("\r\n");
    uprint("Longitude: ");
    uprinti(gpsData->longitude, LL_POINT_POSITION, LL_DEGREES_POSITION);
    uprintc(gpsData->longitudeDirection);
    uprint("\r\n\n");
    gpsData->valid = 0U;
    gpsData->latitude = 0U;
    gpsData->longitude = 0U;
}
