#include <ESP8266WiFi.h>
#include <WeathYaLib.h>

WeathYaLib wl(35); // // Краснодар

void InitWiFi()
{
	const char * WiFi_Name = "Oleg_Home"; // SSID вашей сети
	const char * WiFi_Pass = "lbvf1234"; // Пароль вашей сети
	WiFi.begin(WiFi_Name, WiFi_Pass);
	while(WiFi.waitForConnectResult() != WL_CONNECTED);
}

void setup() {
	Serial.begin(115200);
	InitWiFi();
	Serial.println("WiFi Ok!");
}

void loop() {
	uint16_t sunRise, sunSet;
	
	wl.getSuriseSunset(&sunRise, &sunSet);

	Serial.println("Sun Rise");
	Serial.print(sunRise/60);
	Serial.print(":");
	Serial.println(sunRise%60);

	Serial.println("Sun Set");
	Serial.print(sunSet/60);
	Serial.print(":");
	Serial.println(sunSet%60);
	
	Serial.println("Weather");
	Serial.println(wl.getWeatherString());

	delay(5000);
}
