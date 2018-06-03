Библиотека для работы с погодой в Arduino ESP8266

Как использовать:

#include <WeatherLib.h>

WeatherLib wl("Krasnodar"); // "moscow,RU" если в разных странах есть одинаковые названия

void setup() {
	Serial.begin( 115200 );
}

void loop() {
	Serial.println( wl.getWeatherString() );

	delay(5000);
}


!!При этом в скетче должно быть установлено соединение с Интернет.
