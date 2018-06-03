#include <WeatherLib.h>

WeatherLib wl("Krasnodar"); // "moscow,RU" если в разных странах есть одинаковые названия

void setup() {
	Serial.begin( 115200 );
}

void loop() {
	Serial.println( wl.getWeatherString() );

	delay(5000);
}
