#ifndef _GPS_H_
#define _GPS_H_

#include "SIM900.h"

class GPSGSM
{
	public:
		void getBattInf(char *str_perc, char *str_vol);
		void getBattTVol(char *str_vol);
		char attachGPS();
		char deattachGPS();
		char getStat();
		char getPar(char *str_long, char *str_lat, char *str_alt, char *str_time, char *speed);
};

#endif
