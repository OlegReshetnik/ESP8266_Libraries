#include <ESP8266WiFi.h>
#include <WeatherLib.h>

WeatherLib wl("Krasnodar"); // "moscow,RU" если в разных странах есть одинаковые названия

void InitWiFi()
{
	const char * WiFi_Name = "********";
	const char * WiFi_Pass = "********";
	WiFi.begin( WiFi_Name, WiFi_Pass );
	while(WiFi.waitForConnectResult() != WL_CONNECTED) WiFi.begin( WiFi_Name, WiFi_Pass );
}

void setup() {
	Serial.begin( 115200 );
	InitWiFi();
}

void loop() {
	Serial.println( wl.getWeatherString() );
	delay(5000);
}
