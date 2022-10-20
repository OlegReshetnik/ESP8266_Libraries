#include <ESP8266WiFi.h>
#include <TimeLib.h>

TimeLib tt;

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
	Serial.print( tt.tmDate() );
	Serial.print( " " );
	Serial.println( tt.tmTime() );

	delay(2000);
}
