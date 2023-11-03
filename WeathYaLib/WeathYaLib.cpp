#include "WeathYaLib.h"
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <pgmspace.h>

WeathYaLib::WeathYaLib(const int city) {
	_city = (uint16_t)city;
	_pWeatherString = (char *)malloc(MAX_WEATHER_STRING);
	*_pWeatherString = 0;
	_oldRead = millis() - (WEATHER_UPDATE_INTERVAL * 2);
}

WeathYaLib::~WeathYaLib() {
	free(_pWeatherString);
}

char * WeathYaLib::getWeatherString() {
	_updateWeather();
	return _pWeatherString;
}

void WeathYaLib::getSuriseSunset(uint16_t *sunRise, uint16_t *sunSet) {
	_updateWeather();
	*sunRise = _sunRise;
	*sunSet = _sunSet;
}

void WeathYaLib::_updateWeather(void) {
	if( (millis() - _oldRead) > WEATHER_UPDATE_INTERVAL) {
		char out[4][32];
		_getWeather(out);
		_pWeatherString[0] = 0;
		if (out[2][0] != '-') { strcat(_pWeatherString, "+"); }
		strcat(_pWeatherString, out[2]);
		strcat(_pWeatherString, "C ");
		_parseIcon(out[3], _pWeatherString);
		_sunRise = (out[0][0]-'0') * 600 + (out[0][1]-'0') * 60 + (out[0][3]-'0') * 10 + (out[0][4]-'0');
		_sunSet = (out[1][0]-'0') * 600 + (out[1][1]-'0') * 60 + (out[1][3]-'0') * 10 + (out[1][4]-'0');
		_oldRead = millis();
	}
}

static const char sk[] PROGMEM = "sk ясно";
static const char ns[] PROGMEM = "ns мало облачно";
static const char fe[] PROGMEM = "fe меньше облачно";
static const char sc[] PROGMEM = "sc облачно";
static const char bk[] PROGMEM = "bk сильно облачно";
static const char ov[] PROGMEM = "ov пасмурно";
static const char dz[] PROGMEM = "dz морось";
static const char ra[] PROGMEM = "ra дождь";
static const char sn[] PROGMEM = "sn снег";
static const char bl[] PROGMEM = "bl метель";
static const char gr[] PROGMEM = "gr град";
static const char fg[] PROGMEM = "fg туман";
static const char ts[] PROGMEM = "ts гроза";
const char* const wzs[] PROGMEM = {sk, ns, fe, sc, bk, ov, dz, ra, sn, bl, gr, fg, ts};
#define N_WEATHERS		13

void WeathYaLib::_parseIcon(char *icon, char *out) {
	while(1) {
		for (int i=0; i<N_WEATHERS; i++) {
			char * ptr = (char*)pgm_read_dword(&wzs[i]);
			if (strncasecmp_P(icon, ptr, 2) == 0) { strcat_P(out, &ptr[3]); break; }
		}
		while (*icon != '-') { if (*icon++==0) { return; } }
		icon++;
		strcat(out, " ");
	}
}

// https://yandex.com/time/sync.json?geo=35
// {"time":1648119977119,"clocks":{"35":{"id":35,"name":"Краснодар","offset":10800000,"offsetString":"UTC+3:00",
// "showSunriseSunset":true,"sunrise":"06:20","sunset":"18:40","isNight":false,"skyColor":"#63befa",
// "weather":{"temp":7,"icon":"bkn-d","link":"https://yandex.ru/pogoda/35?lat=45.03547&lon=38.975313"},
// "parents":[{"id":10995,"name":"Краснодарский край"},{"id":225,"name":"Россия"}]}}}
const char sr[] PROGMEM = "rise\":\"";
const char ss[] PROGMEM = "nset\":\"";
const char tt[] PROGMEM = "temp\":";
const char ii[] PROGMEM = "icon\":\"";
const char* const what[] PROGMEM = {sr, ss, tt, ii};
#define N_WHATS		4


void WeathYaLib::_getWeather(char out[4][32]) {
	const uint16_t ssl_port = 443;
	const char w_host[] = "yandex.com";
	bool isSearch = true;
 	int idx = 0, i = 0;

	for (int j=0; j<N_WHATS; j++) { out[j][0] = 0; }
	char * ptr = (char*)pgm_read_dword(&what[0]); // idx = 0

	WiFiClientSecure client;
	client.setInsecure();
	if (client.connect(w_host, ssl_port)) {
		os_sprintf(_pWeatherString, "GET /time/sync.json?geo=%d HTTP/1.1\nHost: %s\n\n", _city, w_host);
		client.print(_pWeatherString);
		while (client.connected()) {
			if (client.available()) {
				char c = client.read();
				if (isSearch) {
					if (c == pgm_read_byte(&ptr[i++])) {
						if (pgm_read_byte(&ptr[i]) == 0) {
							isSearch = false;
							i = 0;
						}
					} else { i = 0; }
				} else {
					if (c != ',' && c != '"' && i<31) { out[idx][i++] = c; }
					else {
						out[idx][i] = 0;
						i = 0;
						if (++idx == N_WHATS) { client.stop(); break; }
						else {
							isSearch = true;
							ptr = (char*)pgm_read_dword(&what[idx]);
						}
					}
				}
			}
		}
	}
}
