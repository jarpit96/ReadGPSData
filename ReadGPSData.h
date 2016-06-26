#define GPSFORMAT 0
#define UTCTIME 1
#define LATITUDE 2
#define LATITUDEHEM 3
#define LONGITUDE 4
#define LONGITUDEHEM 5
#define QUALITY 6
#define NUMSAT 7
#define PRECISION 8
#define ALTITUDE 9
#define GEOIDALHEIGHT 10
#define RUTCTIME 1
#define RSTATUS 2
#define RLATITUDE 3
#define RLATITUDEHEM 4
#define RLONGITUDE 5
#define RLONGITUDEHEM 6
#define RGROUNDSPEED 8  //7
#define RBEARING 7    //8
#define RUTCDATE 9
#define RMAGVAR 10
#define RMAGVARDIR 11
#define deg_to_rad 0.0174603174
#define rad_to_deg 57.27272727
#include<string.h>
#include<sstream>

class gps{
	
    public:
    char defined_ports[4][15];
    char port[15], data[15][20];
    char next_byte;
    int port_status;
    char *token;
    char utctime[12],longitudehem[8],latitudehem[8],longstr[14], latstr[14],altstr[14],bearstr[14],prev_latstr[14],prev_longstr[14],prev_latitudehem[8],prev_longitudehem[8],prev_utctime[8],prev_bearstr[14],prev_altstr[14];
    
        gps()
        {
        strcpy(port,"/dev/ttyUSB0");
        strcpy(defined_ports[0],"/dev/ttyUSB0");
        strcpy(defined_ports[1],"/dev/ttyUSB1");
        strcpy(defined_ports[2],"/dev/ttyACM0");
        strcpy(defined_ports[3],"/dev/ttyUSB2");
        port_status=0;
        }
        
        int initport(char s[]);
        int findport();
        int verifyport();
        int openport(char s[20]);
        int openport();
        void closeport();
        int update();
        int extract(char a[]);
        int setvalues();
        void return_prev();
        void calc_bearstr();
};

	
