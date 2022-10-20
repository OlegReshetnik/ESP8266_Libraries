#pragma once
#include <Arduino.h>


#define WEATHER_UPDATE_INTERVAL	(15*60*1000) // 15 minutes

#define MAX_WEATHER_STRING	80


class WeathYaLib {
	public:
		WeathYaLib(const int city);
		~WeathYaLib();
		char * getWeatherString();
		void getSuriseSunset(uint16_t *sunRise, uint16_t *sunSet);

	private:
		void _updateWeather(void);
		void _parseIcon(char *icon, char *out);
		void _getWeather(char out[4][32]);
		uint16_t _city, _sunRise, _sunSet;
		uint32_t _oldRead;
		char * _pWeatherString;
};
