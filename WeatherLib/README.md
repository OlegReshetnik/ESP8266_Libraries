# Библиотека для работы с погодой в Arduino ESP8266

Как использовать:

#include <WeatherLib.h>

WeatherLib wl("City","api_key"); // "City,RU" если в разных странах есть одинаковые названия

// Получить API key здесь: https://openweathermap.org/appid


Serial.println( wl.getWeatherString() );

Также можно использовать функцию getWeather()

которая возвращает указатель на структуру:

struct weatherSt {

	uint8_t		temp;

	uint8_t		humidity;

	uint8_t		wind_speed;

	uint8_t		wind_dir;

	uint16_t	pressure;

	char		description[(DESK_LEN+1)];

};

Поле wind_dir содержит одно из следующих значений:

W_W - западный ветер

W_WS - юг-западный

W_S - южный

W_SO - юго-восточный

W_O - восточный

W_NO - северо-восточный

W_N - северный

W_NW - северо-западный

!!В скетче должно быть установлено соединение с Интернет. 
