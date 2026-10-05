#include "../../hal/UART.h"
#include "../../components/nmea_gps/nmea_gps_parser.h"
#include "S32K312.h"


int main(void) {

    UART6_Init(9600);
    gps_data_t gpsData;
    GPS_parser_init(&gpsData);
    
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
