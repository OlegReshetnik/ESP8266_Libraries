#include <ESP8266WiFi.h>
#include <WeatherLib.h>

WeatherLib wl("City","api_key"); // "City,RU" если в разных странах есть одинаковые названия
// Получить API key здесь: https://openweathermap.org/appid

void InitWiFi()
{
	const char * WiFi_Name = "********"; // SSID вашей сети
	const char * WiFi_Pass = "********"; // Пароль вашей сети
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
