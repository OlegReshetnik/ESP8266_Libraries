#include <ESP8266WiFi.h>
#include <TimeLib.h>

TimeLib tt; // Например: TimeLib tt(2); - Калининград.
            //Зона по умолчанию - 3 (Москва)

void InitWiFi()
{
	const char * WiFi_Name = "********";
	const char * WiFi_Pass = "********";
	WiFi.begin( WiFi_Name, WiFi_Pass );
	while(WiFi.waitForConnectResult() != WL_CONNECTED) WiFi.begin( WiFi_Name, WiFi_Pass );
}

void setup()
{
	Serial.begin(115200);
	InitWiFi();
}

void loop()
{
	/* Получить данные:
	struct tm {
	  int	tm_sec;
	  int	tm_min;
	  int	tm_hour;
	  int	tm_mday;
	  int	tm_mon;
	  int	tm_year;
	  int	tm_wday;
	  int	tm_yday;
	  int	tm_isdst; };
	*/
	struct tm c_tine = cTime();
	Serial.printf( "Время с точками: %02d.%02d.%02d\n", c_tine->tm_hour, c_tine->tm_min, c_tine->tm_sec );
	
	Serial.print( tt.tmDate() );
	Serial.print( " " );
	Serial.println( tt.tmTime() );

	delay(2000);
}
