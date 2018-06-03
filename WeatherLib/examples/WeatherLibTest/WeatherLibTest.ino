#include <WeatherLib.h>

WeatherLib wl("Krasnodar");

void setup() {
	Serial.begin( 115200 );
}

void loop() {
	Serial.println( wl.getWeatherString() );

	delay(5000);
}
