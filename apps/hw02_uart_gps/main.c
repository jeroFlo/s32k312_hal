#include "../../hal/UART.h"
#include "S32K312.h"

#define LL_POINT_POSITION 5U

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

typedef struct {
    uint32_t latitude;
    uint32_t longitude;
    char latitudeDirection;
    char longitudeDirection;
    volatile uint8_t valid;
} gps_data_t;

static void GPS_parser(gps_data_t *gpsData, uint8_t c)
{
    static nmea_states_t currentState;
    static uint32_t factor;
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
            case UTC: // dont care about UTC, just skip it
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
                    // capture latitude direction here
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
                    // capture longitude data here
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
                    // capture longitude direction here
                    gpsData->longitudeDirection = c;
                    currentState = LONC;
                }
                break;
        }
}
static void uprinti(uint32_t value, uint8_t pointPosition)
{
    char digits[11];
    uint8_t count = 0U;

    if (value == 0U) {
        uprintc('0');
        return;
    }

    while (value > 0U) {
        digits[count++] = (char)('0' + (value % 10U));
        value /= 10U;
        if (count == pointPosition) {
            digits[count++] = '.';
        }
    }

    while (count > 0U) {
        uprintc(digits[--count]);
    }
}

static void GPS_PrintData(gps_data_t *gpsData)
{
    
    if (0U == gpsData->valid) {
        return;
    }
    
    uprint("\r\n");
    uprint("Latitude: ");
    uprinti(gpsData->latitude, LL_POINT_POSITION);
    uprintc(gpsData->latitudeDirection);
    uprint("\r\n");
    uprint("Longitude: ");
    uprinti(gpsData->longitude, LL_POINT_POSITION);
    uprintc(gpsData->longitudeDirection);
    uprint("\r\n\n");
    gpsData->valid = 0U; // reset valid flag after printing
    gpsData->latitude = 0U; // reset latitude for next reading
    gpsData->longitude = 0U; // reset longitude for next reading
}

static void GPS_ProcessRx(gps_data_t *gpsData)
{
    uint8_t c;
    while(ringBuffer_pop(&rxBuffer, &c)){
        GPS_parser(gpsData, c);
    }
}

int main(void) {

    UART6_Init(9600);
    gps_data_t gpsData;
    gpsData.latitude = 0U;
    gpsData.longitude = 0U;
    gpsData.latitudeDirection = 'I';
    gpsData.longitudeDirection = 'I';
    gpsData.valid = 0U;
    
    uprint("UART OK\r\n");

    while(1)
    {
//        char c = ugetc();
//        uprintc(c+1);
        GPS_ProcessRx(&gpsData);
        GPS_PrintData(&gpsData);
    }
    return 0;
}
