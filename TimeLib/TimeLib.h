#ifndef TIME_LIB_h
#define TIME_LIB_h

#include <Arduino.h>

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
		TimeLib(uint8_t timezone = 3) { configTime(timezone*3600, 0, "pool.ntp.org", "time.nist.gov", "time1.google.com"); }
		uint32_t uinixTime() { return (uint32_t)time(nullptr); }
		struct tm * cTime();
		char * tmDate();
		char * tmShortDate();
		char * tmTime();
		char * tmShortTime();

	private:
		char dateBuff[32];
		char timeBuff[12];
		struct tm __tt;
};

#endif
