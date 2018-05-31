#include  "TimeLib.h"

extern "C" {
	struct tm* sntp_localtime(const time_t *clock);
}


TimeLib::TimeLib( uint8_t timezone )
{
	sntp_stop();
	sntp_setservername( 0, "pool.ntp.org" );
	sntp_setservername( 1, "time.nist.gov" );
	sntp_setservername( 2, "time1.google.com" );
	sntp_set_timezone( timezone );
	sntp_init();
}

uint32_t TimeLib::uinixTime()
{
	return (uint32_t)sntp_get_current_timestamp();
}

struct tm * TimeLib::cTime()
{
	uint32_t clk = sntp_get_current_timestamp();
	__tt = sntp_localtime( (const time_t *)&clk );
	return __tt;
}

char * TimeLib::tmDate( struct tm *tim_p )
{
	static const char *day_name[] = {"Воскресенье","Понедельник","Вторник","Среда","Четверг","Пятница","Суббота"};
	static const char *mon_name[] = {"Январь","Февраль","Март","Апрель","Май","Июнь","Июль","Август","Сентябрь","Октябрь","Ноябрь","Декабрь"};
	os_sprintf( dateBuff, "%s %02d %s %d", day_name[tim_p->tm_wday], tim_p->tm_mday, mon_name[tim_p->tm_mon], 1900 + tim_p->tm_year );
	return dateBuff;
}

char * TimeLib::tmDate()
{
	return tmDate( cTime() );
}

char * TimeLib::tmTime( struct tm *tim_p )
{
	os_sprintf( timeBuff, "%02d:%02d:%02d", tim_p->tm_hour, tim_p->tm_min, tim_p->tm_sec );
	return timeBuff;
}

char * TimeLib::tmTime()
{
	return tmTime( cTime() );
}
