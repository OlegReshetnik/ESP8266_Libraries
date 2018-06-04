#include "WeatherLib.h"

WeatherLib::WeatherLib( const char * city, const char * apiKey ) {
	_pCity = (char *)city;
	_pApiKey = (char *)apiKey;
	_pClient = new WiFiClient;
	_pWeatherString = (char *)malloc(MAX_WEATHER_STRING);
	_pWeather = new weatherSt;
	_oldRead = millis() - ( WEATHER_UPDATE_INTERVAL * 2 );
	_updateWeather();
}

WeatherLib::~WeatherLib() {
	delete _pWeather;
	free( _pWeatherString );
	delete _pClient;
}

void WeatherLib::_updateWeather() {
	if( ( millis() - _oldRead ) > WEATHER_UPDATE_INTERVAL ) {
		char * buff = (char *)malloc(WEATHER_READ_BUFF);
		if( _getJson( _pClient, buff, _pCity, _pApiKey ) ) {
			_parseJson( buff, _pWeather );
			_oldRead = millis();
		}
		free(buff);
	}
}

weatherSt * WeatherLib::getWeather() {
	_updateWeather();
	return _pWeather;
}

char * WeatherLib::getWeatherString() {
	_updateWeather();
	_printWeather( _pWeatherString, _pWeather );
	return _pWeatherString;
}

void WeatherLib::_printWeather( char * str, weatherSt * _wz ) {
	const char *wind[8] = {"Зап","Юго-Зап","Южн","Юго-Вост","Вост","Сев-Вост","Сев","Сев-Зап"};
	sprintf( str, "%s%dC %d%% %dмм %s %dм/с %s",
	(_wz->temp>0)?"+":"",
	_wz->temp,
	_wz->humidity,
	_wz->pressure,
	wind[_wz->wind_dir],
	_wz->wind_speed,
	_wz->description );
	
}

bool WeatherLib::_getJson( WiFiClient * pClnt, char * buff, char * city, char * apiKey ) {
	const char * weatherHost = "api.openweathermap.org";
	if( pClnt->connect( weatherHost, 80) ) {
		sprintf( buff,
		"GET /data/2.5/weather?q=%s&units=metric&appid=%s&lang=ru\r\nHost: %s\r\nUser-Agent: ArduinoWiFi/1.1\r\nConnection: close\r\n\r\n",
		city, apiKey, weatherHost );
		pClnt->print( buff );
		uint8_t cnt = 0;
		while( !pClnt->available() && cnt++ < 10 ) delay(500);
		int nn = pClnt->read( (uint8_t *)buff, WEATHER_READ_BUFF );
		if( nn > 0 ) {
			buff[nn] = 0;
			return true;
		}
	}
	return false;
}

void WeatherLib::_parseJson( char * buff, weatherSt * _wz ) {
	const char _temp[] = "temp";
	const char _humidity[] = "humidity";
	const char _pressure[] = "pressure";
	const char _speed[] = "speed";
	const char _deg[] = "deg";
	const char _description[] = "description";

	if( buff[0] != 0 ) {
		char * p;
		p = strstr( buff, _temp ); if( p ) _wz->temp = (uint8_t)atoi( p+strlen(_temp)+2 ); else _wz->temp=0;
		p = strstr( buff, _humidity ); if( p ) _wz->humidity = (uint8_t)atoi( p+strlen(_humidity)+2 ); else _wz->humidity=0;
		p = strstr( buff, _pressure ); if( p ) _wz->pressure = (uint16_t)( 0.750062 * (float)atoi( p+strlen(_pressure)+2 ) ); else _wz->pressure=0;
		p = strstr( buff, _speed ); if( p ) _wz->wind_speed = (uint8_t)atoi( p+strlen(_speed)+2 ); else _wz->wind_speed=0;
		p = strstr( buff, _deg );
		if( p ) {
			uint16_t tmp = (uint16_t)atoi( p+strlen(_deg)+2 );
			if( tmp > 22 && tmp <= 68 ) _wz->wind_dir = W_WS;
			else if( tmp > 68 && tmp <= 113 ) _wz->wind_dir = W_S;
			else if( tmp > 113 && tmp <= 158 ) _wz->wind_dir = W_SO;
			else if( tmp > 158 && tmp <= 203 ) _wz->wind_dir = W_O;
			else if( tmp > 203 && tmp <= 248 ) _wz->wind_dir = W_NO;
			else if( tmp > 248 && tmp <= 293 ) _wz->wind_dir = W_N;
			else if( tmp > 293 && tmp <= 338 ) _wz->wind_dir = W_NW;
			else _wz->wind_dir = W_W;
		}
		else _wz->wind_dir=0;
		p = strstr( buff, _description ); if( p ) {
			p += strlen(_description)+3;
			uint8_t i = 0;
			while( *p != '"' && i < DESK_LEN ) _wz->description[i++] = *p++;
			_wz->description[i] = 0;
		}
		else _wz->description[0] = 0;
	}
}
