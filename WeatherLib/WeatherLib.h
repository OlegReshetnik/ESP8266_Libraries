#ifndef WEATHER_LIB_H
#define WEATHER_LIB_H

#include <ESP8266WiFi.h>
#include <WiFiClient.h>

#define WEATHER_UPDATE_INTERVAL	(20*60*1000) // 20 minutes

#define W_W		0 // западный
#define W_WS	1 // юг-западный
#define W_S		2 // южный
#define W_SO	3 // юго-восточный
#define W_O		4 // восточный
#define W_NO	5 // северо-восточный
#define W_N		6 // северный
#define W_NW	7 // северо-западный

#define DESK_LEN			31
#define WEATHER_READ_BUFF	300
#define MAX_WEATHER_STRING	64

struct weatherSt {
	uint8_t		temp;
	uint8_t		humidity;
	uint8_t		wind_speed;
	uint8_t		wind_dir;
	uint16_t	pressure;
	char		description[(DESK_LEN+1)];
};

class WeatherLib {
	public:
		WeatherLib( const char * city, const char * apiKey );
		~WeatherLib();
		weatherSt * getWeather();
		char * getWeatherString();

	private:
		void _updateWeather();
		void _printWeather( char * str, weatherSt * _wz );
		bool _getJson( WiFiClient * pClnt, char * buff, char * city, char * apiKey );
		void _parseJson( char * buff, weatherSt * _wz );

		WiFiClient * _pClient;
		weatherSt * _pWeather;
		char * _pWeatherString;
		char * _pCity;
		char * _pApiKey;
		uint32_t _oldRead;
};


#endif
