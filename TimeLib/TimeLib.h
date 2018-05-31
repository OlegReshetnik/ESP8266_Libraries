#ifndef TIME_LIB_h
#define TIME_LIB_h

#include <Arduino.h>
#include <time.h>
#include <sys/time.h>
#include <sys/reent.h>
#include "sntp.h"
#include "coredecls.h"

/*
struct tm
{
  int	tm_sec;
  int	tm_min;
  int	tm_hour;
  int	tm_mday;
  int	tm_mon;
  int	tm_year;
  int	tm_wday;
  int	tm_yday;
  int	tm_isdst;
};
*/


class TimeLib
{
	public:
		TimeLib( uint8_t timezone = 3 );
		uint32_t uinixTime();
		struct tm * cTime();
		char * tmDate( struct tm *tim_p );
		char * tmDate();
		char * tmTime( struct tm *tim_p );
		char * tmTime();

	private:
		char dateBuff[32];
		char timeBuff[12];
		struct tm * __tt;
};

#endif