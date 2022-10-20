#include <time.h>
#include  "TimeLib.h"

struct tm * TimeLib::cTime() {
	time_t ts = time(nullptr);
	localtime_r(&ts, &__tt);
	return &__tt;
}

static const char *__day_name[] = {"Вс","Пн","Вт","Ср","Чт","Пт","Сб"};

char * TimeLib::tmDate() {
	struct tm *tim_p = cTime();
	static const char *mon_name[] = {"Янв","Фев","Мар","Апр","Май","Июн","Июл","Авг","Сен","Окт","Ноя","Дек"};
	os_sprintf(dateBuff, "%s %d %s %d", __day_name[tim_p->tm_wday], tim_p->tm_mday, mon_name[tim_p->tm_mon], 1900 + tim_p->tm_year );
	return dateBuff;
}

char * TimeLib::tmShortDate() {
	struct tm *tim_p = cTime();
	os_sprintf(dateBuff, "%02d.%02d %s", tim_p->tm_mday, 1+tim_p->tm_mon, __day_name[tim_p->tm_wday]);
	return dateBuff;
}

char * TimeLib::tmTime() {
	struct tm *tim_p = cTime();
	os_sprintf(timeBuff, "%02d:%02d:%02d", tim_p->tm_hour, tim_p->tm_min, tim_p->tm_sec);
	return timeBuff;
}

char * TimeLib::tmShortTime() {
	struct tm *tim_p = cTime();
	os_sprintf(timeBuff, "%02d:%02d", tim_p->tm_hour, tim_p->tm_min);
	return timeBuff;
}
